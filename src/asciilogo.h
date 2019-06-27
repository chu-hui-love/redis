/*
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

char *ascii_logo =
"                       ::\n"
"                      :;J7, :,                        ::;7: \n"
"                      ,ivYi, ,                       ;LLLFS: \n"
"                      :iv7Yi                       :7ri;j5PL      cyzi customer Redis %s (%s/%d) %s bit      \n"
"                     ,:ivYLvr                    ,ivrrirrY2X,     Running in %s mode          \n"
"                     :;r@Wwz.7r:                :ivu@kexianli.    Port: %d  \n"
"                    :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur     PID: %ld   \n"
"                   ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17 \n"
"                ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i \n"
"              :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv, \n"
"                 ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li \n"
"             ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712: \n"
"           :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri \n"
"          ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur: \n"
"        ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv:: \n"
"        :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;:: \n"
"       ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii \n"
"      ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri: \n"
"     ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7: \n"
"    :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii \n"
"    ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,::::::: \n"
"    i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i:: \n"
"    :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::, \n"
"    i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i: \n"
"    :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri:: \n"
"    :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::, \n"
"     :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu., \n"
"          , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii:: \n"
"              :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii: \n"
"           ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i, \n"
"           ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i, \n"
"        , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri:: \n"
"         :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::\n";
/*
"                _._                                                  \n"
"           _.-``__ ''-._                                             \n"
"      _.-``    `.  `_.  ''-._           Redis %s (%s/%d) %s bit\n"
"  .-`` .-```.  ```\\/    _.,_ ''-._                                   \n"
" (    '      ,       .-`  | `,    )     Running in %s mode\n"
" |`-._`-...-` __...-.``-._|'` _.-'|     Port: %d\n"
" |    `-._   `._    /     _.-'    |     PID: %ld\n"
"  `-._    `-._  `-./  _.-'    _.-'                                   \n"
" |`-._`-._    `-.__.-'    _.-'_.-'|                                  \n"
" |    `-._`-._        _.-'_.-'    |           http://redis.io        \n"
"  `-._    `-._`-.__.-'_.-'    _.-'                                   \n"
" |`-._`-._    `-.__.-'    _.-'_.-'|                                  \n"
" |    `-._`-._        _.-'_.-'    |                                  \n"
"  `-._    `-._`-.__.-'_.-'    _.-'                                   \n"
"      `-._    `-.__.-'    _.-'                                       \n"
"          `-._        _.-'                                           \n"
"              `-.__.-'                                               \n\n";*/
