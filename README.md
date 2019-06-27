
Source code layout
---

Redis根目录只包含这个README,Makefile调用`src`目录中的实际Makefile以及Redis和Sentinel的示例配置.您可以找到一些用于执行Redis,Redis Cluster和Redis Sentinel单元测试的shell脚本,这些脚本在`tests`目录中实现.

根目录中有以下重要目录:

* `src`: 包含用C编写的Redis实现.
* `tests`: 包含单元测试, 在Tcl中实现.
* `deps`: 包含Redis使用的库. 编译Redis所需的一切都在这个目录中;你的系统只需要提供`libc`,POSIX兼容接口和C编译器. 
 需要注意的是,`deps`包含了一个`jemalloc`的副本,它是Linux下Redis的默认分配器. 
 请注意,在`deps`下也有从Redis项目开始的东西,但主存储库不是`antirez/redis`.
 此规则的一个例外是`deps/geohash-int`,它是Redis使用的低级地理编码库:它起源于一个不同的项目,但在这一点上,它的分歧如此之大,以至于它被开发为直接位于Redis存储库中的一个独立实体.

还有一些目录,但是它们对我们的目标不是很重要.
我们将主要关注`src`,其中包含Redis实现,探索每个文件中的内容.文件公开的顺序是逻辑上的顺序,以便逐步公开不同层次的复杂性.

注意: 最近Redis被重构了很多.
函数名称和文件名已更改,因此您可能会发现此文档更密切地反映了`unstable`分支.
例如在Redis 3.0中,`server.c`和`server.h`文件名为`redis.c`和`redis.h`.
但总体结构是一样的.
请记住,所有新开发和pull请求都应该针对`unstable`分支执行.

server.h
---

理解程序如何工作的最简单方法是理解它使用的数据结构.所以我们将从Redis的主头文件`server.h`开始.

所有服务器配置和一般所有共享状态都在名为`server`的全局结构中定义,类型为`struct redisServer`.
这个结构中的一些重要字段是:

* `server.db` 是一个Redis数据库数组，其中存储了数据.
* `server.commands` 是命令表.
* `server.clients` 是连接到服务器的客户端的链接列表.
* `server.master` 是一个特殊的客户端,如果实例是副本,则为master.

还有很多其他字段.大多数字段都直接在结构定义中注释.

另一个重要的Redis数据结构是定义客户端的数据结构.
在过去它被称为`redisClient`,现在只是`client`.
这个结构体有很多字段,这里将只显示主要的那些:

    struct client {
        int fd;
        sds querybuf;
        int argc;
        robj **argv;
        redisDb *db;
        int flags;
        list *reply;
        char buf[PROTO_REPLY_CHUNK_BYTES];
        ... many other fields ...
    }

client结构体定义的是 *已连接的客户端*:

* `fd` 字段是客户端socket文件描述符.
* `argc` and `argv` 使用客户端正在执行的命令填充,以便实现给定Redis命令的函数可以读取参数.
* `querybuf` 累积来自客户端的请求,Redis服务器根据Redis协议解析请求,并通过调用客户端正在执行的命令的实现来执行.
* `reply` and `buf` 是动态和静态缓冲区,用于累积服务器发送给客户端的回复.只要文件描述符可写,这些缓冲区就会逐渐写入套接字.

正如您在上面的客户端结构中所看到的,命令中的参数被描述为`robj`结构.
以下是完整的`robj`结构,它定义了一个*Redis object*
 
    typedef struct redisObject {
        unsigned type:4;
        unsigned encoding:4;
        unsigned lru:LRU_BITS; /* lru time (relative to server.lruclock) */
        int refcount;
        void *ptr;
    } robj;

基本上,这个结构可以表示所有基本的Redis数据类型,如字符串,列表,集合,有序集等.
有趣的是,它有一个`type`字段,这样就可以知道给定对象的类型,还有一个`refcount`,这样同一个对象就可以在多个地方引用,而不需要多次分配它.最后,`ptr`字段指向对象的实际表示,即使对于相同的类型,它也可能会根据使用的`encoding`而有所不同.

Redis对象在Redis内部广泛使用,
但为了避免间接访问的开销,最近在很多地方我们只使用未包装在Redis对象中的普通动态字符串.

server.c
---

这是Redis服务器的入口点,在这里定义了`main()`函数.以下是启动Redis服务器的最重要步骤.

* `initServerConfig()`设置`server`结构的默认值.
* `initServer()`分配操作,设置监听套接字等所需的数据结构.
* `aeMain()`启动事件循环,侦听新连接.

事件循环定期调用两个特殊函数:

1. `serverCron()` 被周期性的调用(根据`server.hz`频率),执行必须需要不定时执行的任务,例如检查timedout客户端.
2. 每次触发事件循环时都会调用`beforeSleep()`,Redis会处理一些请求,并返回到事件循环中.

在server.c内部,可以找到处理Redis服务器其他重要事项的代码:

* `call()`用于在给定客户端的上下文中调用给定命令.
* `activeExpireCycle()`通过`EXPIRE`命令处理Key的驱逐时间.
* 当应该执行一个新的写命令,但根据`maxmemory`指令,Redis内存不足时,调用`freeMemoryIfNeeded()`
* 全局变量`redisCommandTable`定义所有Redis命令,指定命令的名称,实现命令的函数,所需的参数数量以及每个命令的其他属性.

networking.c
---

此文件定义了客户端,主服务器和副本的所有I/O功能(在Redis中只是特殊客户端):

* `createClient()` 分配并且初始化新的客户端.
* 命令实现使用`addReply*()`函数系列,以便将数据附加到客户端结构,该数据将作为对执行的给定命令的回复传输到客户端.
* `writeToClient()`将输出缓冲区中待处理的数据发送到客户端,并由*可写事件处理器*`sendReplyToClient()`调用。
* `readQueryFromClient()` 是*可读事件处理器*,并将从客户端读取的数据累积到查询缓冲区中.
* `processInputBuffer()` 是根据Redis协议解析客户端查询缓冲区的入口点.一旦命令准备好被处理,它就会调用`processCommand()`,它在`server.c`中定义,以便实际执行命令.
* `freeClient()`释放、断开连接并删除客户端.

aof.c and rdb.c
---

从名称中可以猜到,这些文件实现了Redis的RDB和AOF持久性. Redis使用基于`fork()`系统调用的持久性模型来创建具有与主Redis线程相同(共享)内存内容的线程. 这个辅助线程转储磁盘上的内存内容.`rdb.c`和`aof.c`使用它来创建磁盘上的快照,以便在仅附加文件太大时执行AOF重写.

`aof.c`中的实现有额外的功能,以便实现一个API,该API允许命令在客户端执行新命令时将新命令附加到aof文件中.

在`server.c`中定义的`call()`函数负责调用该函数,然后将命令写入AOF.

db.c
---

某些Redis命令对特定数据类型进行操作,其他命令则是通用的.通用命令的示例是`DEL`和`EXPIRE`. 它们专注于关键字,而不是它们的值.所有这些通用命令都在`db.c`中定义.

此外,`db.c`实现了一个API,以便在不直接访问内部数据结构的情况下对Redis数据集执行某些操作.

`db.c`中最重要的功能在许多命令实现中都使用了以下功能:

* `lookupKeyRead()`和`lookupKeyWrite()`用于获取指向与给定key关联的值的指针,如果键不存在则使用`NULL`.
* `dbAdd()`及其更高级别的对应`setKey()`在Redis数据库中创建一个新的key.
* `dbDelete()` 删除一个key和其关联的value.
* `emptyDb()` 删除整个单数据库或定义的所有数据库。.

文件的其余部分实现了向客户端公开的通用命令.

object.c
---

定义Redis对象的`robj`结构已经描述过了.在`object.c`内部,有所有基本级别上与Redis对象操作的函数,比如分配新对象、处理引用计数等函数,这个文件中值得注意的函数:

* `incrRefcount()`和`decrRefCount()`用于递增或递减对象引用计数.当它下降到0时,最终释放对象.
* `createObject()` 分配一个新的对象.还有专门的函数来分配具有特定内容的字符串对象.例如`createStringObjectFromLongLong()`和类似的函数.

该文件还实现了`OBJECT`命令.

replication.c
---

这是Redis中最复杂的文件之一,建议您在稍微熟悉了代码库的其余部分之后再进行阅读.
在此文件中，实现了Redis的主备角色.
这个文件中最重要的函数之一是`replicationfeedslave()`,它向代表连接到我们的主服务器的副本实例的客户端编写命令,以便副本能够获得客户端执行的写操作:
这样,它们的数据集将与主服务器中的数据集保持同步.

这个文件还实现了`SYNC`和`PSYNC`命令,这些命令用于在主服务器和副本之间执行第一次同步,或者在断开连接后继续复制.

其他C文件
---

* `t_hash.c`, `t_list.c`, `t_set.c`, `t_string.c` and `t_zset.c` 包含Redis数据类型的实现. 它们实现了用于访问给定数据类型的API，以及用于这些数据类型的客户端命令实现.
* `ae.c` 实现了Redis事件循环,它是一个独立的库,易于阅读和理解.
* `sds.c` 是Redis字符串库, 有关更多信息,请访问http://github.com/antirez/sds.
* `anet.c` 是一个比内核公开的原始接口更简单且更易于使用POSIX网络的库.
* `dict.c` 是一个非阻塞哈希表的实现,它以递增方式重新哈希.
* `scripting.c` 实现了Lua脚本. 它完全独立于Redis实现的其余部分,如果您熟悉Lua API,那么它非常简单.
* `cluster.c` 实现Redis集群. 可能只有在非常熟悉Redis代码库的其余部分之后，才会有很好的阅读效果. 如果你想读`cluster.c` 请务必阅读[Redis 集群规范][3].

[3]: http://redis.io/topics/cluster-spec

剖析Redis命令
---

所有Redis命令都被定义在下面的路径:

    void foobarCommand(client *c) {
        printf("%s",c->argv[1]->ptr); /* Do something with the argument. */
        addReply(c,shared.ok); /* Reply something to the client. */
    }

然后在命令表的`server.c`中引用该命令:

    {"foobar",foobarCommand,2,"rtF",0,NULL,0,0,0,0,0},

在上面的例子中,`2`是命令所采用的参数个数,而`rtF`是命令标志,如`server.c`中命令表顶部注释中所述.

命令以某种方式运行后,它会向客户端返回一个回复,通常使用`addReply()`或`networking.c`中定义的类似函数.

在Redis源代码中有大量的命令实现,可以作为实际命令实现的示例.编写一些玩具命令可以很好地熟悉代码库.
