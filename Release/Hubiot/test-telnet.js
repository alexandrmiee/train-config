console.log('test telnet start');
tcp.connect('127.0.0.1',23);
tcp.read();
tcp.send('get * b *');
array = [
  'get * b *',
  'get * c *',
  'get * r *',
  'get k b *'
];
var i = 0;
function tcpCallback(res){
  console.log('tcpCallback: '+res);
  thread.sleep(1000);
  tcp.send(array[i]);
  console.log('send: '+array[i]);
  i = i+1;
  if(i>3){
   	tcp.close();
   	console.log('test telnet stop');
   }
};
function tcpError(e){
  console.log('tcpError: '+e);
  tcp.close();
};