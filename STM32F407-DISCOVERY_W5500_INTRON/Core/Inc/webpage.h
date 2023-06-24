#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

/*************************************************************************************
 * JavaScript Functions
 *************************************************************************************/

#define w5x00web_ajax_js 	\
"function AJAX(a,e)"\
"{"\
    "var c=d();"\
    "c.onreadystatechange=b;"\
    "function d()"\
    "{"\
        "if(window.XMLHttpRequest)"\
        "{"\
            "return new XMLHttpRequest()"\
        "}"\
        "else"\
        "{"\
            "if(window.ActiveXObject)"\
            "{"\
                "return new ActiveXObject(\"Microsoft.XMLHTTP\")"\
            "}"\
        "}"\
    "}"\
    "function b()"\
    "{"\
        "if(c.readyState==4)"\
        "{"\
            "if(c.status==200)"\
            "{"\
                "if(e)"\
                "{"\
                    "e(c.responseText)"\
                "}"\
            "}"\
        "}"\
    "}"\
    "this.doGet=function()"\
    "{"\
        "c.open(\"GET\",a,true);"\
        "c.send(null)"\
    "};"\
    "this.doPost=function(f)"\
    "{"\
        "c.open(\"POST\",a,true);"\
        "c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");"\
        "c.setRequestHeader(\"ISAJAX\",\"yes\");"\
        "c.send(f)"\
    "}"\
"}"\
"function $(a)"\
"{"\
    "return document.getElementById(a)"\
"}"\
"function $$(a)"\
"{"\
    "return document.getElementsByName(a)"\
"}"\
"function $$_ie(a,c)"\
"{"\
    "if(!a)"\
    "{"\
        "a=\"*\""\
    "}"\
    "var b=document.getElementsByTagName(a);"\
    "var e=[];"\
    "for(var d=0;d<b.length;d++)"\
    "{"\
        "att=b[d].getAttribute(\"name\");"\
        "if(att==c)"\
        "{"\
            "e.push(b[d])"\
        "}"\
    "}"\
    "return e"\
"}"

/* Get: Network Information: function NetinfoCallback(o), getNetinfo() */
#define w5x00web_netinfo_js	"function NetinfoCallback(o){"\
									 "$('txtmac').value=o.mac;"\
									 "$('txtip').value=o.ip;"\
									 "$('txtgw').value=o.gw;"\
									 "$('txtsn').value=o.sn;"\
									 "$('txtdns').value=o.dns;"\
									 "if(typeof(window.external)!='undefined'){"\
										"obj=$$_ie('input','dhcp');"\
									 "}else{"\
										"obj=$$('dhcp');"\
									 "}"\
								"}"\
								" "\
								"function getNetinfo(){"\
									 "var oUpdate;"\
									 "setTimeout(function(){"\
									 	"oUpdate=new AJAX('get_netinfo.cgi',function(t){"\
									 		"try{eval(t);}catch(e){alert(e);}"\
									 "});"\
									 "oUpdate.doGet();},300);"\
								 "}"


/* Get: Digital I/O state/dir:  function DioCallback(o), getDio(o) */
/* Set: Digital I/O state: 		function setDiostate(o) */
/* Set: Digital I/O direction: 	function setDiodir(o) */
#define w5x00web_dio_js	"function DioCallback(o){"\
								"var pin = o.dio_p;"\
								"$('txtdio_s'+pin).value=o.dio_s;"\
								"$('txtdio_d'+pin).value=o.dio_d;"\
							"}"\
							"function getDio(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"oUpdate=new AJAX('get_dio'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();"\
							"}"\
							" "\
							"function setDiostate(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_s'+p).value;*/"\
								"var v=o.attributes['s'].value;"\
								"dout=new AJAX('set_diostate.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"\
							" "\
							"function setDiodir(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_d'+p).value;*/"\
								"var v=o.attributes['d'].value;"\
								"dout=new AJAX('set_diodir.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"



/*************************************************************************************
 * HTML Pages (web pages)
 *************************************************************************************/

const char index_page[] =
        "<!DOCTYPE html> "
        "<html lang='ru'>"
        "  <head> "
        "  <meta charset='utf-8'>"
        "  <title>Авторизация</title>"
        "  </head>"
        "  <body> "
        "  <br><form onsubmit='return false;' id='frm1' >"
        "    <label for='login'>Логин:&nbsp;&nbsp;&nbsp;</label> "
        "    <input type='text' id='login' name='login' value=''><br>"
        "  </form><br>"
        "  <form onsubmit='return false;' id='frm2' >"
        "    <label for='password'>Пароль:&nbsp;</label>"
        "    <input type='password' id='password' name='password' value=''><br>"
        "  </form> <br>"
        "<button onclick='send()'>Вход</button>"
        "<script>"
        "function sleep(milliseconds) "
        "{"
        "  const date = Date.now();"
        "  let currentDate = null;"
        "  do {"
        "    currentDate = Date.now();"
        "  } while (currentDate - date < milliseconds);"
        "}"
        "function send() "
        "{"
        "  let step1 = new XMLHttpRequest();"
        "  step1.open('GET', 'LOGIN'+document.getElementById('login').value);"
        "  step1.send();"
        "  sleep(300);"
        "  let x2 = document.getElementById('password').value;"
        "  let step2 = new XMLHttpRequest();"
        "  var hash = md5(x2);"
        "  step2.open('GET', 'PASSWORD'+hash);"
        "  step2.send();"
        "  sleep(300);"
        "  let step3 = new XMLHttpRequest();"
        "  step3.open('GET', '/ ');"
        "  location.reload();"
        "}"
        "var md5 = function (str) {"
        "  /*"
        "   * Add integers, wrapping at 2^32. This uses 16-bit operations internally"
        "   * to work around bugs in some JS interpreters."
        "   */"
        "  function safe_add(x, y) {"
        "    var lsw = (x & 0xFFFF) + (y & 0xFFFF),"
        "      msw = (x >> 16) + (y >> 16) + (lsw >> 16);"
        "    return (msw << 16) | (lsw & 0xFFFF);"
        "  }"
        "  /*"
        "   * Bitwise rotate a 32-bit number to the left."
        "   */"
        "  function bit_rol(num, cnt) {"
        "    return (num << cnt) | (num >>> (32 - cnt));"
        "  }"
        "  /*"
        "   * These functions implement the four basic operations the algorithm uses."
        "   */"
        "  function md5_cmn(q, a, b, x, s, t) {"
        "    return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s), b);"
        "  }"
        "  function md5_ff(a, b, c, d, x, s, t) {"
        "    return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);"
        "  }"
        "  function md5_gg(a, b, c, d, x, s, t) {"
        "    return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);"
        "  }"
        "  function md5_hh(a, b, c, d, x, s, t) {"
        "    return md5_cmn(b ^ c ^ d, a, b, x, s, t);"
        "  }"
        "  function md5_ii(a, b, c, d, x, s, t) {"
        "    return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);"
        "  }"
        "  /*"
        "   * Calculate the MD5 of an array of little-endian words, and a bit length."
        "   */"
        "  function binl_md5(x, len) {"
        "    /* append padding */"
        "    x[len >> 5] |= 0x80 << ((len) % 32);"
        "    x[(((len + 64) >>> 9) << 4) + 14] = len;"
        "    var i, olda, oldb, oldc, oldd,"
        "      a = 1732584193,"
        "      b = -271733879,"
        "      c = -1732584194,"
        "      d = 271733878;"
        "    for (i = 0; i < x.length; i += 16) {"
        "      olda = a;"
        "      oldb = b;"
        "      oldc = c;"
        "      oldd = d;"
        "      a = md5_ff(a, b, c, d, x[i], 7, -680876936);"
        "      d = md5_ff(d, a, b, c, x[i + 1], 12, -389564586);"
        "      c = md5_ff(c, d, a, b, x[i + 2], 17, 606105819);"
        "      b = md5_ff(b, c, d, a, x[i + 3], 22, -1044525330);"
        "      a = md5_ff(a, b, c, d, x[i + 4], 7, -176418897);"
        "      d = md5_ff(d, a, b, c, x[i + 5], 12, 1200080426);"
        "      c = md5_ff(c, d, a, b, x[i + 6], 17, -1473231341);"
        "      b = md5_ff(b, c, d, a, x[i + 7], 22, -45705983);"
        "      a = md5_ff(a, b, c, d, x[i + 8], 7, 1770035416);"
        "      d = md5_ff(d, a, b, c, x[i + 9], 12, -1958414417);"
        "      c = md5_ff(c, d, a, b, x[i + 10], 17, -42063);"
        "      b = md5_ff(b, c, d, a, x[i + 11], 22, -1990404162);"
        "      a = md5_ff(a, b, c, d, x[i + 12], 7, 1804603682);"
        "      d = md5_ff(d, a, b, c, x[i + 13], 12, -40341101);"
        "      c = md5_ff(c, d, a, b, x[i + 14], 17, -1502002290);"
        "      b = md5_ff(b, c, d, a, x[i + 15], 22, 1236535329);"
        "      a = md5_gg(a, b, c, d, x[i + 1], 5, -165796510);"
        "      d = md5_gg(d, a, b, c, x[i + 6], 9, -1069501632);"
        "      c = md5_gg(c, d, a, b, x[i + 11], 14, 643717713);"
        "      b = md5_gg(b, c, d, a, x[i], 20, -373897302);"
        "      a = md5_gg(a, b, c, d, x[i + 5], 5, -701558691);"
        "      d = md5_gg(d, a, b, c, x[i + 10], 9, 38016083);"
        "      c = md5_gg(c, d, a, b, x[i + 15], 14, -660478335);"
        "      b = md5_gg(b, c, d, a, x[i + 4], 20, -405537848);"
        "      a = md5_gg(a, b, c, d, x[i + 9], 5, 568446438);"
        "      d = md5_gg(d, a, b, c, x[i + 14], 9, -1019803690);"
        "      c = md5_gg(c, d, a, b, x[i + 3], 14, -187363961);"
        "      b = md5_gg(b, c, d, a, x[i + 8], 20, 1163531501);"
        "      a = md5_gg(a, b, c, d, x[i + 13], 5, -1444681467);"
        "      d = md5_gg(d, a, b, c, x[i + 2], 9, -51403784);"
        "      c = md5_gg(c, d, a, b, x[i + 7], 14, 1735328473);"
        "      b = md5_gg(b, c, d, a, x[i + 12], 20, -1926607734);"
        "      a = md5_hh(a, b, c, d, x[i + 5], 4, -378558);"
        "      d = md5_hh(d, a, b, c, x[i + 8], 11, -2022574463);"
        "      c = md5_hh(c, d, a, b, x[i + 11], 16, 1839030562);"
        "      b = md5_hh(b, c, d, a, x[i + 14], 23, -35309556);"
        "      a = md5_hh(a, b, c, d, x[i + 1], 4, -1530992060);"
        "      d = md5_hh(d, a, b, c, x[i + 4], 11, 1272893353);"
        "      c = md5_hh(c, d, a, b, x[i + 7], 16, -155497632);"
        "      b = md5_hh(b, c, d, a, x[i + 10], 23, -1094730640);"
        "      a = md5_hh(a, b, c, d, x[i + 13], 4, 681279174);"
        "      d = md5_hh(d, a, b, c, x[i], 11, -358537222);"
        "      c = md5_hh(c, d, a, b, x[i + 3], 16, -722521979);"
        "      b = md5_hh(b, c, d, a, x[i + 6], 23, 76029189);"
        "      a = md5_hh(a, b, c, d, x[i + 9], 4, -640364487);"
        "      d = md5_hh(d, a, b, c, x[i + 12], 11, -421815835);"
        "      c = md5_hh(c, d, a, b, x[i + 15], 16, 530742520);"
        "      b = md5_hh(b, c, d, a, x[i + 2], 23, -995338651);"
        "      a = md5_ii(a, b, c, d, x[i], 6, -198630844);"
        "      d = md5_ii(d, a, b, c, x[i + 7], 10, 1126891415);"
        "      c = md5_ii(c, d, a, b, x[i + 14], 15, -1416354905);"
        "      b = md5_ii(b, c, d, a, x[i + 5], 21, -57434055);"
        "      a = md5_ii(a, b, c, d, x[i + 12], 6, 1700485571);"
        "      d = md5_ii(d, a, b, c, x[i + 3], 10, -1894986606);"
        "      c = md5_ii(c, d, a, b, x[i + 10], 15, -1051523);"
        "      b = md5_ii(b, c, d, a, x[i + 1], 21, -2054922799);"
        "      a = md5_ii(a, b, c, d, x[i + 8], 6, 1873313359);"
        "      d = md5_ii(d, a, b, c, x[i + 15], 10, -30611744);"
        "      c = md5_ii(c, d, a, b, x[i + 6], 15, -1560198380);"
        "      b = md5_ii(b, c, d, a, x[i + 13], 21, 1309151649);"
        "      a = md5_ii(a, b, c, d, x[i + 4], 6, -145523070);"
        "      d = md5_ii(d, a, b, c, x[i + 11], 10, -1120210379);"
        "      c = md5_ii(c, d, a, b, x[i + 2], 15, 718787259);"
        "      b = md5_ii(b, c, d, a, x[i + 9], 21, -343485551);"
        "      a = safe_add(a, olda);"
        "      b = safe_add(b, oldb);"
        "      c = safe_add(c, oldc);"
        "      d = safe_add(d, oldd);"
        "    }"
        "    return [a, b, c, d];"
        "  }"
        "  /*"
        "   * Convert a raw string to an array of little-endian words"
        "   * Characters >255 have their high-byte silently ignored."
        "   */"
        "  function rstr2binl(input) {"
        "    var i,"
        "      output = [];"
        "    output[(input.length >> 2) - 1] = undefined;"
        "    for (i = 0; i < output.length; i += 1) {"
        "      output[i] = 0;"
        "    }"
        "    for (i = 0; i < input.length * 8; i += 8) {"
        "      output[i >> 5] |= (input.charCodeAt(i / 8) & 0xFF) << (i % 32);"
        "    }"
        "    return output;"
        "  }"
        "  /*"
        "   * Convert an array of little-endian words to a string"
        "   */"
        "  function binl2rstr(input) {"
        "    var i,"
        "      output = '';"
        "    for (i = 0; i < input.length * 32; i += 8) {"
        "      output += String.fromCharCode((input[i >> 5] >>> (i % 32)) & 0xFF);"
        "    }"
        "    return output;"
        "  }"
        "  /*"
        "   * Convert a raw string to a hex string"
        "   */"
        "  function rstr2hex(input) {"
        "    var hex_tab = '0123456789abcdef',"
        "      output = '',"
        "      x,"
        "      i;"
        "    for (i = 0; i < input.length; i += 1) {"
        "      x = input.charCodeAt(i);"
        "      output += hex_tab.charAt((x >>> 4) & 0x0F) +"
        "        hex_tab.charAt(x & 0x0F);"
        "    }"
        "    return output;"
        "  }"
        "  return function (str) {"
        "    str = unescape(encodeURIComponent(str));"
        "    str = binl_md5(rstr2binl(str), str.length * 8);"
        "    str = binl2rstr(str);"
        "    return rstr2hex(str);"
        "  }"
        "}()"
        "</script></body></html>"
;

const char main_page[] =
        "<!DOCTYPE html> "
        "<html lang='ru'>"
        "  <head> "
        "  <meta charset='utf-8'>"
        "  <h2 align='center'>GIT - Группа индустриальных технологий</h2> "
        "  <title>Мост IDSN<->IP</title>"
        "  </head>"
        "  <body> "
        "  <h3>Настройка моста ISDN - IP</h3> "
        "  <form onsubmit='return false;' id='frm1' >"
        "    <label for='host_ip' >IP адрес хоста:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label> "
        "    <input type='text' id='host_ip' name='host_ip' value='' style='width: 110px;'>"
        "    <input type='button' onclick='send1()' value='обновить' style='"
        "    width: 80px;'>"
        "  </form><br>"
        "  <form onsubmit='return false;' id='frm2' >"
        "    <label for='mask' style=>Маска сети:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label>"
        "    <input type='text' id='mask' name='mask' value='' style='width: 110px;'>"
        "    <input type='button' onclick='send2()' value='обновить' style='"
        "    width: 80px;'>"
        "  </form><br> "
        "  <form onsubmit='return false;' id='frm3' >"
        "    <label for='gateway' >IP адрес шлюза:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label>"
        "    <input type='text' id='gateway' name='gateway' value='' style='width: 110px;'>"
        "    <input type='button' onclick='send3()' value='обновить' style='"
        "    width: 80px;'>"
        "  </form><br> "
        "  <form onsubmit='return false;' id='frm4' >"
        "    <label for='dest_ip' >IP адрес получателя:</label>"
        "    <input type='text' id='dest_ip' name='dest_ip' value='' style='width: 110px;'>"
        "    <input type='button' onclick='send4()' value='обновить' style='"
        "    width: 80px;'>"
        "  </form><br>"
        "<button onclick='reboot()' style='border-left-width: 2px;border-left-style: solid;margin-left: 138px;'>Перезагруза хоста</button>"
        "<p id='demo' value='  '></p>"
        "<script>"
        "  function sleep(milliseconds) "
        "  {"
        "    const date = Date.now();"
        "    let currentDate = null;"
        "    do {"
        "      currentDate = Date.now();"
        "    } while (currentDate - date < milliseconds);"
        "  };"
        "  sleep(300);"
        "  let request1 = new XMLHttpRequest();"
        "  request1.open('GET', '/host_IP');"
        "  request1.send();"
        "  sleep(300);"
        "  request1.onreadystatechange = function() "
        "  {"
        "    document.getElementById('frm1').elements[0].value=request1.responseText.toString().substring(0, 15);"
        "  };"
        "  let request2 = new XMLHttpRequest();"
        "  sleep(100);"
        "  request2.open('GET', '/dest_IP');"
        "  request2.send();"
        "  sleep(300);"
        "  request2.onreadystatechange = function() "
        "  {"
        "    document.getElementById('frm4').elements[0].value=request2.responseText.toString().substring(0, 15);"
        "  };"
        "  let request3 = new XMLHttpRequest();"
        "  sleep(100);"
        "  request3.open('GET', '/gate_IP');"
        "  request3.send();"
        "  sleep(300);"
        "  request3.onreadystatechange = function() "
        "  {"
        "    document.getElementById('frm3').elements[0].value=request3.responseText.toString().substring(0, 15);"
        "  };"
        "  let request4 = new XMLHttpRequest();"
        "  sleep(100);"
        "  request4.open('GET', '/mask_IP');"
        "  request4.send();"
        "  sleep(300);"
        "  request4.onreadystatechange = function() "
        "  {"
        "    document.getElementById('frm2').elements[0].value=request4.responseText.toString().substring(0, 15);"
        "  };"
        "  function ValidateIPaddress(ipaddress) "
        "  {  "
        "    if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) "
        "    {  "
        "      return (true); "
        "    }  "
        "  return (false);  "
        "  }; "
        "  let req = new XMLHttpRequest();"
        "  function send1() "
        "  {"
        "    let command = 'SET_HOST_IP';"
        "    let x1 = document.getElementById('frm1').elements[0].value;"
        "    if(ValidateIPaddress(x1))"
        "    {"
        "      req.open('GET', command+x1);"
        "      req.send();"
        "      sleep(2000);"
        "      document.getElementById('demo').innerHTML = 'IP адрес хоста обновлен';"
        "    }"
        "    else"
        "    {"
        "      document.getElementById('demo').innerHTML = 'Некоррекнтый IP адрес';"
        "      document.getElementById('frm1').elements[0].value = '';"
        "      return false;"
        "    }"
        "  };                                               "
        "  function send2() "
        "  {"
        "    let command = 'SET_MASK_IP';"
        "    let x2 = document.getElementById('frm2').elements[0].value;"
        "    if(ValidateIPaddress(x2))"
        "    {"
        "      req.open('GET', command+x2);"
        "      req.send();"
        "      sleep(2000);"
        "      document.getElementById('demo').innerHTML = 'Маска сети обновлена';"
        "    }"
        "    else"
        "    {"
        "      document.getElementById('demo').innerHTML = 'Некорректная маска';"
        "      document.getElementById('frm2').elements[0].value = '';"
        "      return false;"
        "    }"
        "  };"
        "  function send3() "
        "  {"
        "    let command = 'SET_GATE_IP';"
        "    let x3 = document.getElementById('frm3').elements[0].value;"
        "    if(ValidateIPaddress(x3))"
        "    {"
        "      req.open('GET', command+x3);"
        "      req.send();"
        "      sleep(2000);"
        "      document.getElementById('demo').innerHTML = 'IP адрес шлюза обновлен';"
        "    }"
        "    else"
        "    {"
        "      document.getElementById('demo').innerHTML = 'Hекоррекнтый IP адрес';"
        "      document.getElementById('frm3').elements[0].value = '';"
        "      return false;"
        "    }"
        "  };"
        "  function send4() "
        "  {"
        "    let command = 'SET_DEST_IP';"
        "    let x4 = document.getElementById('frm4').elements[0].value;"
        "    if(ValidateIPaddress(x4))"
        "    {"
        "      req.open('GET', command+x4);"
        "      req.send();"
        "      sleep(2000);"
        "      document.getElementById('demo').innerHTML = 'IP адрес получателя обновлен';"
        "    }"
        "    else"
        "    {"
        "      document.getElementById('demo').innerHTML = 'Некоррекнтый IP адрес';"
        "      document.getElementById('frm4').elements[0].value = '';"
        "      return false;"
        "    }"
        "  };"
        "  function reboot() "
        "  {"
        "    let command = 'REBOOT';"
        "      req.open('GET', command);"
        "      req.send();"
        "      sleep(2000);"
        "      document.getElementById('demo').innerHTML = 'Хост перезагружен';"
        "  };"
        "  var md5 = function (str) "
        "  {"
        "    /*"
        "     * Add integers, wrapping at 2^32. This uses 16-bit operations internally"
        "     * to work around bugs in some JS interpreters."
        "     */"
        "    function safe_add(x, y) "
        "    {"
        "      var lsw = (x & 0xFFFF) + (y & 0xFFFF), msw = (x >> 16) + (y >> 16) + (lsw >> 16);"
        "      return (msw << 16) | (lsw & 0xFFFF);"
        "    };"
        "    /*"
        "     * Bitwise rotate a 32-bit number to the left."
        "     */"
        "    function bit_rol(num, cnt) "
        "    {"
        "      return (num << cnt) | (num >>> (32 - cnt));"
        "    };"
        "    /*"
        "     * These functions implement the four basic operations the algorithm uses."
        "     */"
        "    function md5_cmn(q, a, b, x, s, t) "
        "    {"
        "      return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s), b);"
        "    };"
        "    function md5_ff(a, b, c, d, x, s, t) "
        "    {"
        "      return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);"
        "    };"
        "    function md5_gg(a, b, c, d, x, s, t) "
        "    {"
        "      return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);"
        "    };"
        "    function md5_hh(a, b, c, d, x, s, t) "
        "    {"
        "       return md5_cmn(b ^ c ^ d, a, b, x, s, t);"
        "    };"
        "    function md5_ii(a, b, c, d, x, s, t) "
        "    {"
        "      return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);"
        "    };"
        "    /*"
        "     * Calculate the MD5 of an array of little-endian words, and a bit length."
        "     */"
        "    function binl_md5(x, len) "
        "    {"
        "      /* append padding */"
        "      x[len >> 5] |= 0x80 << ((len) % 32);"
        "      x[(((len + 64) >>> 9) << 4) + 14] = len;"
        "      var i, olda, oldb, oldc, oldd,"
        "        a = 1732584193,"
        "        b = -271733879,"
        "        c = -1732584194,"
        "        d = 271733878;"
        "      for (i = 0; i < x.length; i += 16) "
        "      {"
        "        olda = a;"
        "        oldb = b;"
        "        oldc = c;"
        "        oldd = d;"
        "        a = md5_ff(a, b, c, d, x[i], 7, -680876936);"
        "        d = md5_ff(d, a, b, c, x[i + 1], 12, -389564586);"
        "        c = md5_ff(c, d, a, b, x[i + 2], 17, 606105819);"
        "        b = md5_ff(b, c, d, a, x[i + 3], 22, -1044525330);"
        "        a = md5_ff(a, b, c, d, x[i + 4], 7, -176418897);"
        "        d = md5_ff(d, a, b, c, x[i + 5], 12, 1200080426);"
        "        c = md5_ff(c, d, a, b, x[i + 6], 17, -1473231341);"
        "        b = md5_ff(b, c, d, a, x[i + 7], 22, -45705983);"
        "        a = md5_ff(a, b, c, d, x[i + 8], 7, 1770035416);"
        "        d = md5_ff(d, a, b, c, x[i + 9], 12, -1958414417);"
        "        c = md5_ff(c, d, a, b, x[i + 10], 17, -42063);"
        "        b = md5_ff(b, c, d, a, x[i + 11], 22, -1990404162);"
        "        a = md5_ff(a, b, c, d, x[i + 12], 7, 1804603682);"
        "        d = md5_ff(d, a, b, c, x[i + 13], 12, -40341101);"
        "        c = md5_ff(c, d, a, b, x[i + 14], 17, -1502002290);"
        "        b = md5_ff(b, c, d, a, x[i + 15], 22, 1236535329);"
        "        a = md5_gg(a, b, c, d, x[i + 1], 5, -165796510);"
        "        d = md5_gg(d, a, b, c, x[i + 6], 9, -1069501632);"
        "        c = md5_gg(c, d, a, b, x[i + 11], 14, 643717713);"
        "        b = md5_gg(b, c, d, a, x[i], 20, -373897302);"
        "        a = md5_gg(a, b, c, d, x[i + 5], 5, -701558691);"
        "        d = md5_gg(d, a, b, c, x[i + 10], 9, 38016083);"
        "        c = md5_gg(c, d, a, b, x[i + 15], 14, -660478335);"
        "        b = md5_gg(b, c, d, a, x[i + 4], 20, -405537848);"
        "        a = md5_gg(a, b, c, d, x[i + 9], 5, 568446438);"
        "        d = md5_gg(d, a, b, c, x[i + 14], 9, -1019803690);"
        "        c = md5_gg(c, d, a, b, x[i + 3], 14, -187363961);"
        "        b = md5_gg(b, c, d, a, x[i + 8], 20, 1163531501);"
        "        a = md5_gg(a, b, c, d, x[i + 13], 5, -1444681467);"
        "        d = md5_gg(d, a, b, c, x[i + 2], 9, -51403784);"
        "        c = md5_gg(c, d, a, b, x[i + 7], 14, 1735328473);"
        "        b = md5_gg(b, c, d, a, x[i + 12], 20, -1926607734);"
        "        a = md5_hh(a, b, c, d, x[i + 5], 4, -378558);"
        "        d = md5_hh(d, a, b, c, x[i + 8], 11, -2022574463);"
        "        c = md5_hh(c, d, a, b, x[i + 11], 16, 1839030562);"
        "        b = md5_hh(b, c, d, a, x[i + 14], 23, -35309556);"
        "        a = md5_hh(a, b, c, d, x[i + 1], 4, -1530992060);"
        "        d = md5_hh(d, a, b, c, x[i + 4], 11, 1272893353);"
        "        c = md5_hh(c, d, a, b, x[i + 7], 16, -155497632);"
        "        b = md5_hh(b, c, d, a, x[i + 10], 23, -1094730640);"
        "        a = md5_hh(a, b, c, d, x[i + 13], 4, 681279174);"
        "        d = md5_hh(d, a, b, c, x[i], 11, -358537222);"
        "        c = md5_hh(c, d, a, b, x[i + 3], 16, -722521979);"
        "        b = md5_hh(b, c, d, a, x[i + 6], 23, 76029189);"
        "        a = md5_hh(a, b, c, d, x[i + 9], 4, -640364487);"
        "        d = md5_hh(d, a, b, c, x[i + 12], 11, -421815835);"
        "        c = md5_hh(c, d, a, b, x[i + 15], 16, 530742520);"
        "        b = md5_hh(b, c, d, a, x[i + 2], 23, -995338651);"
        "        a = md5_ii(a, b, c, d, x[i], 6, -198630844);"
        "        d = md5_ii(d, a, b, c, x[i + 7], 10, 1126891415);"
        "        c = md5_ii(c, d, a, b, x[i + 14], 15, -1416354905);"
        "        b = md5_ii(b, c, d, a, x[i + 5], 21, -57434055);"
        "        a = md5_ii(a, b, c, d, x[i + 12], 6, 1700485571);"
        "        d = md5_ii(d, a, b, c, x[i + 3], 10, -1894986606);"
        "        c = md5_ii(c, d, a, b, x[i + 10], 15, -1051523);"
        "        b = md5_ii(b, c, d, a, x[i + 1], 21, -2054922799);"
        "        a = md5_ii(a, b, c, d, x[i + 8], 6, 1873313359);"
        "        d = md5_ii(d, a, b, c, x[i + 15], 10, -30611744);"
        "        c = md5_ii(c, d, a, b, x[i + 6], 15, -1560198380);"
        "        b = md5_ii(b, c, d, a, x[i + 13], 21, 1309151649);"
        "        a = md5_ii(a, b, c, d, x[i + 4], 6, -145523070);"
        "        d = md5_ii(d, a, b, c, x[i + 11], 10, -1120210379);"
        "        c = md5_ii(c, d, a, b, x[i + 2], 15, 718787259);"
        "        b = md5_ii(b, c, d, a, x[i + 9], 21, -343485551);"
        "        a = safe_add(a, olda);"
        "        b = safe_add(b, oldb);"
        "        c = safe_add(c, oldc);"
        "        d = safe_add(d, oldd);"
        "      }"
        "      return [a, b, c, d];"
        "    };"
        "    /*"
        "     * Convert a raw string to an array of little-endian words"
        "     * Characters >255 have their high-byte silently ignored."
        "     */"
        "    function rstr2binl(input) "
        "    {"
        "      var i, output = [];"
        "      output[(input.length >> 2) - 1] = undefined;"
        "      for (i = 0; i < output.length; i += 1) {"
        "        output[i] = 0;"
        "      }"
        "      for (i = 0; i < input.length * 8; i += 8) {"
        "        output[i >> 5] |= (input.charCodeAt(i / 8) & 0xFF) << (i % 32);"
        "      }"
        "      return output;"
        "    };"
        "    /*"
        "     * Convert an array of little-endian words to a string"
        "     */"
        "    function binl2rstr(input) "
        "    {"
        "      var i, output = '';"
        "      for (i = 0; i < input.length * 32; i += 8) {"
        "        output += String.fromCharCode((input[i >> 5] >>> (i % 32)) & 0xFF);"
        "      }"
        "      return output;"
        "    };"
        "    /*"
        "     * Convert a raw string to a hex string"
        "     */"
        "    function rstr2hex(input) "
        "    {"
        "      var hex_tab = '0123456789abcdef',"
        "        output = '',"
        "        x,"
        "        i;"
        "      for (i = 0; i < input.length; i += 1) {"
        "        x = input.charCodeAt(i);"
        "        output += hex_tab.charAt((x >>> 4) & 0x0F) +"
        "          hex_tab.charAt(x & 0x0F);"
        "      }"
        "      return output;"
        "    };"
        "    return function (str) "
        "    {"
        "      str = unescape(encodeURIComponent(str));"
        "      str = binl_md5(rstr2binl(str), str.length * 8);"
        "      str = binl2rstr(str);"
        "      return rstr2hex(str);"
        "    };"
        "  }();"
        "  function change_pass() "
        "  {"
        "    var input1 = prompt('Введите пароль', '');"
        "    if (input1 != null)"
        "    {"
        "      var input2 = prompt('Подтвердите пароль', '');"
        "      if (input1 == input2)"
        "      {"
        "        let md5_newpass = md5(input1);"
        "        let command = 'SET_PASSWORD';"
        "        req.open('GET', command+md5_newpass);"
        "        req.send();"
        "        sleep(2000);"
        "        document.getElementById('demo').innerHTML = 'Пароль установлен';"
        "      }"
        "    }"
        "   };"
        "</script>"
        "   <button  onclick='change_pass()' style='border-left-width: 2px;border-left-style: solid;margin-left: 138px; width: 130px'>Сменить пароль</button>"
        "    <style>"
        "      footer {"
        "        padding: 10px 20px;"
        "        background: #666;"
        "        color: white;"
        "      }"
        "    </style>"
        "   <footer>"
        "      <p>Компания © GIT. Все права защищены.  +7 495 223-07-25   119571 Москва,  проспект Вернадского, д.94, корп.5</p>"
        "    </footer>"
        "</html>"
;

//Перезаписываются при загрузке параметров с SD или EEPROM
char host_IP[16] = "192.168.001.222";
char dest_IP[16] = "192.168.001.200";
char gate_IP[16] = "192.168.001.001";
char mask_IP[16] = "255.255.255.000";

//Код WIZNET
//                              "<html>"\
//								"<head>"\
//								"<title>w5x00 Web Server</title>"\
//								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
//								"</head>"\
//								"<body>"\
//									"<div>"\
//									"w5x00 Web Server Demopage"\
//									"</div>"\
//									"<br>"\
//									"<a href='netinfo.html'>Network Information</a>"\
//									"<br>"\
//									"<a href='img.html'>Base64 Image Data</a>"\
//									"<br>"\
//									"<a href='dio.html'> Digital I/O RGB LED</a>"\
//								"</body>"\
//							"</html>"


#define netinfo_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>w5x00 Web Server Network Info</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<style>"\
								"label{float:left;text-align:left;width:50px;}"\
								"li {list-style:none;}"\
								"</style>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='netinfo.js'></script>"\
								"</head>"\
								"<body onload='getNetinfo();'>"\
									"<div>"\
									"w5x00 Web Server Network Information"\
									"</div>"\
									"<br>"\
									"<ul>"\
										"<li><label for='txtmac'>MAC:</label><input id='txtmac' name='mac' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtip'>IP:</label><input id='txtip' name='ip' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtgw'>GW:</label><input id='txtgw' name='gw' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtsn'>SN:</label><input id='txtsn' name='sn' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtdns'>DNS:</label><input id='txtdns' name='dns' type='text' size='20' disabled='disabled'/></li> "\
									"</ul>"\
								"</body>"\
							"</html>"


#define dio_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>w5x00 Web Server Digital I/O</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='dio.js'></script>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
                                    "<input type='button' value='LED G Off' pin='0' s='0' onclick='setDiostate(this);'> "\
                                    "<input type='button' value='LED G On ' pin='0' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
                                    "<input type='button' value='LED O Off' pin='1' s='0' onclick='setDiostate(this);'> "\
                                    "<input type='button' value='LED O On ' pin='1' s= '1' onclick='setDiostate(this);'>"\
                                    "<br>"\
                                    "<input type='button' value='LED B Off' pin='3' s='0' onclick='setDiostate(this);'> "\
                                    "<input type='button' value='LED B On ' pin='3' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
                                    "<input type='button' value='LED R Off' pin='2' s='0' onclick='setDiostate(this);'> "\
                                    "<input type='button' value='LED R On' pin='2' s= '1' onclick='setDiostate(this);'>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"


/* Base64 Image converter: http://base64.wutils.com/encoding-online/ */
#define img_page			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>w5x00 Web Server Image</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
								"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"</head>"\
								"<body>"\
									"<div>"\
									"<img alt='wizwebserver.png'"\
									"src='data:image/png;base64,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' />"\
									"</div>"\
								"</body>"\
							"</html>"



//#define index_page2 "<!DOCTYPE html> <html lang='ru'>  <head>   <meta charset='utf-8'>  <title>Авторизация</title>  </head>  <body>   <br><form onsubmit='return false;' id='frm1' >    <label for='login'>Логин:&nbsp;&nbsp;&nbsp;</label>     <input type='text' id='login' name='login' value=''><br>  </form><br>  <form onsubmit='return false;' id='frm2' >    <label for='password'>Пароль:&nbsp;</label>    <input type='password' id='password' name='password' value=''><br>  </form> <br><button onclick='send()'>Вход</button><script>function sleep(milliseconds) {  const date = Date.now();  let currentDate = null;  do {    currentDate = Date.now();  } while (currentDate - date < milliseconds);}function send() {  let x1 = document.getElementById('login').value;  let step1 = new XMLHttpRequest();  step1.open('GET', 'l'+x1);  step1.send();  sleep(300);  let x2 = document.getElementById('password').value;  let step2 = new XMLHttpRequest();  var hash = md5(x2);  step2.open('GET', 'p'+hash);  step2.send();  sleep(300);  let step3 = new XMLHttpRequest();  step3.open('GET', '/ ');  location.reload();}var md5 = function (str) {  /*   * Add integers, wrapping at 2^32. This uses 16-bit operations internally   * to work around bugs in some JS interpreters.   */  function safe_add(x, y) {    var lsw = (x & 0xFFFF) + (y & 0xFFFF),      msw = (x >> 16) + (y >> 16) + (lsw >> 16);    return (msw << 16) | (lsw & 0xFFFF);  }  /*   * Bitwise rotate a 32-bit number to the left.   */  function bit_rol(num, cnt) {    return (num << cnt) | (num >>> (32 - cnt));  }  /*   * These functions implement the four basic operations the algorithm uses.   */  function md5_cmn(q, a, b, x, s, t) {    return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s), b);  }  function md5_ff(a, b, c, d, x, s, t) {    return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);  }  function md5_gg(a, b, c, d, x, s, t) {    return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);  }  function md5_hh(a, b, c, d, x, s, t) {    return md5_cmn(b ^ c ^ d, a, b, x, s, t);  }  function md5_ii(a, b, c, d, x, s, t) {    return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);  }  /*   * Calculate the MD5 of an array of little-endian words, and a bit length.   */  function binl_md5(x, len) {    /* append padding */    x[len >> 5] |= 0x80 << ((len) % 32);    x[(((len + 64) >>> 9) << 4) + 14] = len;    var i, olda, oldb, oldc, oldd,      a = 1732584193,      b = -271733879,      c = -1732584194,      d = 271733878;    for (i = 0; i < x.length; i += 16) {      olda = a;      oldb = b;      oldc = c;      oldd = d;      a = md5_ff(a, b, c, d, x[i], 7, -680876936);      d = md5_ff(d, a, b, c, x[i + 1], 12, -389564586);      c = md5_ff(c, d, a, b, x[i + 2], 17, 606105819);      b = md5_ff(b, c, d, a, x[i + 3], 22, -1044525330);      a = md5_ff(a, b, c, d, x[i + 4], 7, -176418897);      d = md5_ff(d, a, b, c, x[i + 5], 12, 1200080426);      c = md5_ff(c, d, a, b, x[i + 6], 17, -1473231341);      b = md5_ff(b, c, d, a, x[i + 7], 22, -45705983);      a = md5_ff(a, b, c, d, x[i + 8], 7, 1770035416);      d = md5_ff(d, a, b, c, x[i + 9], 12, -1958414417);      c = md5_ff(c, d, a, b, x[i + 10], 17, -42063);      b = md5_ff(b, c, d, a, x[i + 11], 22, -1990404162);      a = md5_ff(a, b, c, d, x[i + 12], 7, 1804603682);      d = md5_ff(d, a, b, c, x[i + 13], 12, -40341101);      c = md5_ff(c, d, a, b, x[i + 14], 17, -1502002290);      b = md5_ff(b, c, d, a, x[i + 15], 22, 1236535329);      a = md5_gg(a, b, c, d, x[i + 1], 5, -165796510);      d = md5_gg(d, a, b, c, x[i + 6], 9, -1069501632);      c = md5_gg(c, d, a, b, x[i + 11], 14, 643717713);      b = md5_gg(b, c, d, a, x[i], 20, -373897302);      a = md5_gg(a, b, c, d, x[i + 5], 5, -701558691);      d = md5_gg(d, a, b, c, x[i + 10], 9, 38016083);      c = md5_gg(c, d, a, b, x[i + 15], 14, -660478335);      b = md5_gg(b, c, d, a, x[i + 4], 20, -405537848);      a = md5_gg(a, b, c, d, x[i + 9], 5, 568446438);      d = md5_gg(d, a, b, c, x[i + 14], 9, -1019803690);      c = md5_gg(c, d, a, b, x[i + 3], 14, -187363961);      b = md5_gg(b, c, d, a, x[i + 8], 20, 1163531501);      a = md5_gg(a, b, c, d, x[i + 13], 5, -1444681467);      d = md5_gg(d, a, b, c, x[i + 2], 9, -51403784);      c = md5_gg(c, d, a, b, x[i + 7], 14, 1735328473);      b = md5_gg(b, c, d, a, x[i + 12], 20, -1926607734);      a = md5_hh(a, b, c, d, x[i + 5], 4, -378558);      d = md5_hh(d, a, b, c, x[i + 8], 11, -2022574463);      c = md5_hh(c, d, a, b, x[i + 11], 16, 1839030562);      b = md5_hh(b, c, d, a, x[i + 14], 23, -35309556);      a = md5_hh(a, b, c, d, x[i + 1], 4, -1530992060);      d = md5_hh(d, a, b, c, x[i + 4], 11, 1272893353);      c = md5_hh(c, d, a, b, x[i + 7], 16, -155497632);      b = md5_hh(b, c, d, a, x[i + 10], 23, -1094730640);      a = md5_hh(a, b, c, d, x[i + 13], 4, 681279174);      d = md5_hh(d, a, b, c, x[i], 11, -358537222);      c = md5_hh(c, d, a, b, x[i + 3], 16, -722521979);      b = md5_hh(b, c, d, a, x[i + 6], 23, 76029189);      a = md5_hh(a, b, c, d, x[i + 9], 4, -640364487);      d = md5_hh(d, a, b, c, x[i + 12], 11, -421815835);      c = md5_hh(c, d, a, b, x[i + 15], 16, 530742520);      b = md5_hh(b, c, d, a, x[i + 2], 23, -995338651);      a = md5_ii(a, b, c, d, x[i], 6, -198630844);      d = md5_ii(d, a, b, c, x[i + 7], 10, 1126891415);      c = md5_ii(c, d, a, b, x[i + 14], 15, -1416354905);      b = md5_ii(b, c, d, a, x[i + 5], 21, -57434055);      a = md5_ii(a, b, c, d, x[i + 12], 6, 1700485571);      d = md5_ii(d, a, b, c, x[i + 3], 10, -1894986606);      c = md5_ii(c, d, a, b, x[i + 10], 15, -1051523);      b = md5_ii(b, c, d, a, x[i + 1], 21, -2054922799);      a = md5_ii(a, b, c, d, x[i + 8], 6, 1873313359);      d = md5_ii(d, a, b, c, x[i + 15], 10, -30611744);      c = md5_ii(c, d, a, b, x[i + 6], 15, -1560198380);      b = md5_ii(b, c, d, a, x[i + 13], 21, 1309151649);      a = md5_ii(a, b, c, d, x[i + 4], 6, -145523070);      d = md5_ii(d, a, b, c, x[i + 11], 10, -1120210379);      c = md5_ii(c, d, a, b, x[i + 2], 15, 718787259);      b = md5_ii(b, c, d, a, x[i + 9], 21, -343485551);      a = safe_add(a, olda);      b = safe_add(b, oldb);      c = safe_add(c, oldc);      d = safe_add(d, oldd);    }    return [a, b, c, d];  }  /*   * Convert a raw string to an array of little-endian words   * Characters >255 have their high-byte silently ignored.   */  function rstr2binl(input) {    var i,      output = [];    output[(input.length >> 2) - 1] = undefined;    for (i = 0; i < output.length; i += 1) {      output[i] = 0;    }    for (i = 0; i < input.length * 8; i += 8) {      output[i >> 5] |= (input.charCodeAt(i / 8) & 0xFF) << (i % 32);    }    return output;  }  /*   * Convert an array of little-endian words to a string   */  function binl2rstr(input) {    var i,      output = '';    for (i = 0; i < input.length * 32; i += 8) {      output += String.fromCharCode((input[i >> 5] >>> (i % 32)) & 0xFF);    }    return output;  }  /*   * Convert a raw string to a hex string   */  function rstr2hex(input) {    var hex_tab = '0123456789abcdef',      output = '',      x,      i;    for (i = 0; i < input.length; i += 1) {      x = input.charCodeAt(i);      output += hex_tab.charAt((x >>> 4) & 0x0F) +        hex_tab.charAt(x & 0x0F);    }    return output;  }  return function (str) {    str = unescape(encodeURIComponent(str))    str = binl_md5(rstr2binl(str), str.length * 8)    str = binl2rstr(str)    return rstr2hex(str)  }}()</script></body> </html>"
//#define main_page "<!DOCTYPE html> <html lang='ru'>  <head>   <meta charset='utf-8'>  <h2 align='center'>GIT - Группа индустриальных технологий</h2>   <title>Мост IDSN<->IP</title>  </head>  <body>   <h3>Настройка моста ISDN - IP</h3>   <form onsubmit='return false;' id='frm1' >    <label for='host_ip' >IP адрес хоста:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label>     <input type='text' id='host_ip' name='host_ip' value='' style='width: 110px;'>    <input type='button' onclick='send1()' value='обновить' style='    width: 80px;'>  </form><br>  <form onsubmit='return false;' id='frm2' >    <label for='mask' style=>Маска сети:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label>    <input type='text' id='mask' name='mask' value='' style='width: 110px;'>    <input type='button' onclick='send2()' value='обновить' style='    width: 80px;'>  </form><br>   <form onsubmit='return false;' id='frm3' >    <label for='gateway' >IP адрес шлюза:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label>    <input type='text' id='gateway' name='gateway' value='' style='width: 110px;'>    <input type='button' onclick='send3()' value='обновить' style='    width: 80px;'>  </form><br>   <form onsubmit='return false;' id='frm4' >    <label for='dest_ip' >IP адрес получателя:</label>    <input type='text' id='dest_ip' name='dest_ip' value='' style='width: 110px;'>    <input type='button' onclick='send4()' value='обновить' style='    width: 80px;'>  </form><br><button onclick='reboot()' style='border-left-width: 2px;border-left-style: solid;margin-left: 138px;'>Перезагруза хоста</button><p id='demo' value='  '></p><script>  sleep(500);  let request1 = new XMLHttpRequest();  request1.open('GET', '/host_IP');  request1.send();  request1.onreadystatechange = function()   {  var a1=request1.responseText.toString().substring(0, 3)+'.'+request1.responseText.toString().substring(5,8)+'.';  var a2=request1.responseText.toString().substring(10, 13)+'.';  var a3=request1.responseText.toString().substring(15, 18);  var text1 = a1 + a2;  document.getElementById('frm1').elements[0].value=text1 + a3;  }  sleep(500);  let request2 = new XMLHttpRequest();  request2.open('GET', '/dest_IP');  request2.send();  request2.onreadystatechange = function()   {  var b1=request2.responseText.toString().substring(0, 3)+'.'+request2.responseText.toString().substring(5,8)+'.';  var b2=request2.responseText.toString().substring(10, 13)+'.';  var b3=request2.responseText.toString().substring(15, 18);  var text2 = b1 + b2;  document.getElementById('frm4').elements[0].value=text2 + b3;  }  sleep(500);  let request3 = new XMLHttpRequest();  request3.open('GET', '/gate_IP');  request3.send();  request3.onreadystatechange = function()   {  var d1=request3.responseText.toString().substring(0, 3)+'.'+request3.responseText.toString().substring(5,8)+'.';  var d2=request3.responseText.toString().substring(10, 13)+'.';  var d3=request3.responseText.toString().substring(15, 18);  var text4 = d1 + d2;  document.getElementById('frm3').elements[0].value=text4 + d3;  }  sleep(500);  let request4 = new XMLHttpRequest();  request4.open('GET', '/mask_IP');  request4.send();  request4.onreadystatechange = function()   {  var c1=request4.responseText.toString().substring(0, 3)+'.'+request4.responseText.toString().substring(5,8)+'.';  var c2=request4.responseText.toString().substring(10, 13)+'.';  var c3=request4.responseText.toString().substring(15, 18);  var text3 = c1 + c2;  document.getElementById('frm2').elements[0].value=text3 + c3;  }  function sleep(milliseconds)   {    const date = Date.now();    let currentDate = null;    do {      currentDate = Date.now();    } while (currentDate - date < milliseconds);  }  function ValidateIPaddress(ipaddress)   {      if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {        return (true)    }    return (false)    }   let req = new XMLHttpRequest();  function send1()   {    let x1 = document.getElementById('frm1').elements[0].value;    if(ValidateIPaddress(x1))    {      req.open('GET', 1+x1);      req.send();      sleep(2000);      document.getElementById('demo').innerHTML = 'IP адрес хоста обновлен';    }    else    {      document.getElementById('demo').innerHTML = 'Некоррекнтый IP адрес';      document.getElementById('frm1').elements[0].value = '';      return false;    }  }                                                 function send2()   {    let x2 = document.getElementById('frm2').elements[0].value;    if(ValidateIPaddress(x2))    {      req.open('GET', 2+x2);      req.send();      sleep(2000);      document.getElementById('demo').innerHTML = 'Маска сети обновлена';    }    else    {      document.getElementById('demo').innerHTML = 'Некорректная маска';      document.getElementById('frm2').elements[0].value = '';      return false;    }  }  function send3()   {    let x3 = document.getElementById('frm3').elements[0].value;    if(ValidateIPaddress(x3))    {      req.open('GET', 3+x3);      req.send();      sleep(2000);      document.getElementById('demo').innerHTML = 'IP адрес шлюза обновлен';    }    else    {      document.getElementById('demo').innerHTML = 'Hекоррекнтый IP адрес';      document.getElementById('frm3').elements[0].value = '';      return false;    }  }    function send4()   {    let x4 = document.getElementById('frm4').elements[0].value;    if(ValidateIPaddress(x4))    {      req.open('GET', 4+x4);      req.send();      sleep(2000);      document.getElementById('demo').innerHTML = 'IP адрес получателя обновлен';    }    else    {      document.getElementById('demo').innerHTML = 'Некоррекнтый IP адрес';      document.getElementById('frm4').elements[0].value = '';      return false;    }  }  function reboot()   {      req.open('GET', 5);      req.send();      sleep(2000);      document.getElementById('demo').innerHTML = 'Хост перезагружен';  }var md5 = function (str) {  /*   * Add integers, wrapping at 2^32. This uses 16-bit operations internally   * to work around bugs in some JS interpreters.   */  function safe_add(x, y) {    var lsw = (x & 0xFFFF) + (y & 0xFFFF),      msw = (x >> 16) + (y >> 16) + (lsw >> 16);    return (msw << 16) | (lsw & 0xFFFF);  }  /*   * Bitwise rotate a 32-bit number to the left.   */  function bit_rol(num, cnt) {    return (num << cnt) | (num >>> (32 - cnt));  }  /*   * These functions implement the four basic operations the algorithm uses.   */  function md5_cmn(q, a, b, x, s, t) {    return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s), b);  }  function md5_ff(a, b, c, d, x, s, t) {    return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);  }  function md5_gg(a, b, c, d, x, s, t) {    return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);  }  function md5_hh(a, b, c, d, x, s, t) {     return md5_cmn(b ^ c ^ d, a, b, x, s, t);  }  function md5_ii(a, b, c, d, x, s, t) {    return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);  }  /*   * Calculate the MD5 of an array of little-endian words, and a bit length.   */  function binl_md5(x, len) {    /* append padding */    x[len >> 5] |= 0x80 << ((len) % 32);    x[(((len + 64) >>> 9) << 4) + 14] = len;    var i, olda, oldb, oldc, oldd,      a = 1732584193,      b = -271733879,      c = -1732584194,      d = 271733878;    for (i = 0; i < x.length; i += 16) {      olda = a;      oldb = b;      oldc = c;      oldd = d;      a = md5_ff(a, b, c, d, x[i], 7, -680876936);      d = md5_ff(d, a, b, c, x[i + 1], 12, -389564586);      c = md5_ff(c, d, a, b, x[i + 2], 17, 606105819);      b = md5_ff(b, c, d, a, x[i + 3], 22, -1044525330);      a = md5_ff(a, b, c, d, x[i + 4], 7, -176418897);      d = md5_ff(d, a, b, c, x[i + 5], 12, 1200080426);      c = md5_ff(c, d, a, b, x[i + 6], 17, -1473231341);      b = md5_ff(b, c, d, a, x[i + 7], 22, -45705983);      a = md5_ff(a, b, c, d, x[i + 8], 7, 1770035416);      d = md5_ff(d, a, b, c, x[i + 9], 12, -1958414417);      c = md5_ff(c, d, a, b, x[i + 10], 17, -42063);      b = md5_ff(b, c, d, a, x[i + 11], 22, -1990404162);      a = md5_ff(a, b, c, d, x[i + 12], 7, 1804603682);      d = md5_ff(d, a, b, c, x[i + 13], 12, -40341101);      c = md5_ff(c, d, a, b, x[i + 14], 17, -1502002290);      b = md5_ff(b, c, d, a, x[i + 15], 22, 1236535329);      a = md5_gg(a, b, c, d, x[i + 1], 5, -165796510);      d = md5_gg(d, a, b, c, x[i + 6], 9, -1069501632);      c = md5_gg(c, d, a, b, x[i + 11], 14, 643717713);      b = md5_gg(b, c, d, a, x[i], 20, -373897302);      a = md5_gg(a, b, c, d, x[i + 5], 5, -701558691);      d = md5_gg(d, a, b, c, x[i + 10], 9, 38016083);      c = md5_gg(c, d, a, b, x[i + 15], 14, -660478335);      b = md5_gg(b, c, d, a, x[i + 4], 20, -405537848);      a = md5_gg(a, b, c, d, x[i + 9], 5, 568446438);      d = md5_gg(d, a, b, c, x[i + 14], 9, -1019803690);      c = md5_gg(c, d, a, b, x[i + 3], 14, -187363961);      b = md5_gg(b, c, d, a, x[i + 8], 20, 1163531501);      a = md5_gg(a, b, c, d, x[i + 13], 5, -1444681467);      d = md5_gg(d, a, b, c, x[i + 2], 9, -51403784);      c = md5_gg(c, d, a, b, x[i + 7], 14, 1735328473);      b = md5_gg(b, c, d, a, x[i + 12], 20, -1926607734);      a = md5_hh(a, b, c, d, x[i + 5], 4, -378558);      d = md5_hh(d, a, b, c, x[i + 8], 11, -2022574463);      c = md5_hh(c, d, a, b, x[i + 11], 16, 1839030562);      b = md5_hh(b, c, d, a, x[i + 14], 23, -35309556);      a = md5_hh(a, b, c, d, x[i + 1], 4, -1530992060);      d = md5_hh(d, a, b, c, x[i + 4], 11, 1272893353);      c = md5_hh(c, d, a, b, x[i + 7], 16, -155497632);      b = md5_hh(b, c, d, a, x[i + 10], 23, -1094730640);      a = md5_hh(a, b, c, d, x[i + 13], 4, 681279174);      d = md5_hh(d, a, b, c, x[i], 11, -358537222);      c = md5_hh(c, d, a, b, x[i + 3], 16, -722521979);      b = md5_hh(b, c, d, a, x[i + 6], 23, 76029189);      a = md5_hh(a, b, c, d, x[i + 9], 4, -640364487);      d = md5_hh(d, a, b, c, x[i + 12], 11, -421815835);      c = md5_hh(c, d, a, b, x[i + 15], 16, 530742520);      b = md5_hh(b, c, d, a, x[i + 2], 23, -995338651);      a = md5_ii(a, b, c, d, x[i], 6, -198630844);      d = md5_ii(d, a, b, c, x[i + 7], 10, 1126891415);      c = md5_ii(c, d, a, b, x[i + 14], 15, -1416354905);      b = md5_ii(b, c, d, a, x[i + 5], 21, -57434055);      a = md5_ii(a, b, c, d, x[i + 12], 6, 1700485571);      d = md5_ii(d, a, b, c, x[i + 3], 10, -1894986606);      c = md5_ii(c, d, a, b, x[i + 10], 15, -1051523);      b = md5_ii(b, c, d, a, x[i + 1], 21, -2054922799);      a = md5_ii(a, b, c, d, x[i + 8], 6, 1873313359);      d = md5_ii(d, a, b, c, x[i + 15], 10, -30611744);      c = md5_ii(c, d, a, b, x[i + 6], 15, -1560198380);      b = md5_ii(b, c, d, a, x[i + 13], 21, 1309151649);      a = md5_ii(a, b, c, d, x[i + 4], 6, -145523070);      d = md5_ii(d, a, b, c, x[i + 11], 10, -1120210379);      c = md5_ii(c, d, a, b, x[i + 2], 15, 718787259);      b = md5_ii(b, c, d, a, x[i + 9], 21, -343485551);      a = safe_add(a, olda);      b = safe_add(b, oldb);      c = safe_add(c, oldc);      d = safe_add(d, oldd);    }    return [a, b, c, d];  }  /*   * Convert a raw string to an array of little-endian words   * Characters >255 have their high-byte silently ignored.   */  function rstr2binl(input) {    var i,      output = [];    output[(input.length >> 2) - 1] = undefined;    for (i = 0; i < output.length; i += 1) {      output[i] = 0;    }    for (i = 0; i < input.length * 8; i += 8) {      output[i >> 5] |= (input.charCodeAt(i / 8) & 0xFF) << (i % 32);    }    return output;  }  /*   * Convert an array of little-endian words to a string   */  function binl2rstr(input) {    var i,      output = '';    for (i = 0; i < input.length * 32; i += 8) {      output += String.fromCharCode((input[i >> 5] >>> (i % 32)) & 0xFF);    }    return output;  }  /*   * Convert a raw string to a hex string   */  function rstr2hex(input) {    var hex_tab = '0123456789abcdef',      output = '',      x,      i;    for (i = 0; i < input.length; i += 1) {      x = input.charCodeAt(i);      output += hex_tab.charAt((x >>> 4) & 0x0F) +        hex_tab.charAt(x & 0x0F);    }    return output;  }  return function (str) {    str = unescape(encodeURIComponent(str))    str = binl_md5(rstr2binl(str), str.length * 8)    str = binl2rstr(str)    return rstr2hex(str)  }}()  function change_pass()   {    var input1 = prompt('Введите пароль', '');    if (input1 != null)    {      var input2 = prompt('Подтвердите пароль', '');      if (input1 == input2)      {        let md5_newpass = md5(input1);        req.open('GET', 6+md5_newpass);        req.send();        sleep(2000);        document.getElementById('demo').innerHTML = 'Пароль установлен';      }    }      }</script>   <button  onclick='change_pass()' style='border-left-width: 2px;border-left-style: solid;margin-left: 138px; width: 130px'>Сменить пароль</button>    <style>      footer {        padding: 10px 20px;        background: #666;        color: white;      }    </style>   <footer>      <p>Компания © GIT. Все права защищены.  +7 495 223-07-25   119571 Москва,  проспект Вернадского, д.94, корп.5</p>    </footer></html>"



#endif
