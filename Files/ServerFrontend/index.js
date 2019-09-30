window.onload = function() {	
	var FileUpload = Vue.component('upload', 
		{
			template:'#upload-tmpl'
			,data() {
				return {
					url:this.test.url
					,method:this.test.method
					,data:this.test.data
				}
			}
			,props:['test','index']
			,methods:{
				upload(){
					var tt = this;
					var timer = setTimeout(function() { tt.$emit('error','Сервер не ответил в течение 1 с.'); }, 1000);
					send(
						this.url
						,this.method
						,this.data
						,function(msg){
							try{
								console.log(msg);
								var json = JSON.parse(msg);
								tt.$emit('upload',json);
								clearTimeout(timer);
							} catch(e) {
								tt.$emit('error',e);
								clearTimeout(timer);
							}
						}
						,function(e){
							console.log(e);
							tt.$emit('error',e);
						}
					);
				}
				,load(e){
					console.log(e.target.files);
					this.data = e.target.files[0];
				}
			}
		});
	new Vue({
		el:'#app'
		,created :async function() {
			for(var i=0;i<this.tests.length;i++){
				await this.check(this.tests[i]);
			}
			await this.getDb('*','*','*');
			await this.getFs();
		}
		,data(){
			return{
				tests:[
					{
						name:'TCP'
						,result:'Если Вы видите этот текст, значит проверка прошла успешно'
						,url:null
						,method:null
						,data:null
						,isSucces:true
						,protocol:null
					}
					,{
						name:'HTTP GET File'
						,result:'Если Вы видите этот текст, значит проверка прошла успешно'
						,url:null
						,method:null
						,data:null
						,isSucces:true
						,protocol:null
					}
					,{
						name:'HTTP JSON get * * *'
						,result:'идет проверка...'
						,url:'http://127.0.0.1/json/get/*/*/*'
						,method:'GET'
						,data:null
						,protocol:'http'
						,isSucces:null
					}
					,{
						name:'HTTP JSON getv HTTP port *'
						,result:'идет проверка...'
						,url:'http://127.0.0.1/json/getv/HTTP/port/*'
						,method:'GET'
						,data:null
						,protocol:'http'
						,isSucces:null
					}
					,{
						name:'HTTP File ls 6 (DISK_FLASH_MC)'
						,result:'идет проверка...'
						,url:'http://127.0.0.1/dir/6/folder/*'
						,method:'GET'
						,data:null
						,protocol:'http'
						,isSucces:null
					}
					/*,{
						name:'HTTP POST File'
						,result:'Для проверки загрузите любой файл'
						,url:'http://127.0.0.1/save/4/folder/test.txt'
						,method:'POST'
						,data:new File(["HTTP POST File. Test file text."], "test.txt", {type: "text/plain"})
						,protocol:'http'
						,upload:true
						,isSucces:null
					}*/
					,{
						name:'MQTT JSON get * * *'
						,result:'идет проверка...'
						,protocol:'mqtt'
						,topic:'/get'
						,message:'get * * *'
						,isSucces:null
					}
					,{
						name:'MQTT JSON getv HTTP port *'
						,result:'идет проверка...'
						,protocol:'mqtt'
						,topic:'/getv'
						,message:'getv HTTP port *'
						,isSucces:null
					}
					,{
						name:'Telnet'
						,result:'Проверку можно провести только с помощью Telnet клиента, например Putty. Для проверки запустите Putty, подлючитесь к серверу и наберите help.'
						,protocol:null
						,isSucces:null
					}
					,{
						name:'SNMP'
						,result:'Проверку можно провести только с помощью SNMP сервера.'
						,protocol:null
						,isSucces:null
					}
				]
				,items:[
				]
				,drives:[
				]
				,files:{
					flash_db:[]
					,flash_boot:[]
					,flash_bl:[]
					,sd_spi:[]
					,flash_spi:[]
					,flash_can:[]
					,flash_mc:[]
				}
			}
		}
		,components:{
			FileUpload
		}
		,methods:{
			async getDb(object,key,value){
				var vm = this;
				send(
					'http://127.0.0.1/json/get/'+object+'/'+key+'/'+value
					,'GET'
					,''
					,function(msg){
						try{
							vm.items = JSON.parse(msg);
						} catch(e) {
							console.log(e);
						}
					}
					,function(e){
						console.log(e);
					}
				);
			}
			,async getObject(index,object,key,value){
				var vm = this;
				send(
					'http://127.0.0.1/json/get/'+object+'/'+key+'/'+value
					,'GET'
					,''
					,function(msg){
						try{
							vm.items[index] = JSON.parse(msg);
						} catch(e) {
							console.log(e);
						}
					}
					,function(e){
						console.log(e);
					}
				);
			}
			,async postObject(object,key,value,item){
				var json = JSON.stringify(item)
				json = json.replace(/[\u007F-\uFFFF]/g, function(chr) {
					return "\\u" + ("0000" + chr.charCodeAt(0).toString(16)).substr(-4)
				})
				json = json.replace(/"/g,'');
				json = json.replace(/{/,'');
				json = json.replace(/}/,'');
				json = json.replace(/:/,'');
				console.log(json);

				send(
					'http://127.0.0.1/save/0/'+object+'/'+key
					,'POST'
					,new File([json], key, {type: "text/plain"})
					,function(msg){
						console.log(msg);
					}
					,function(e){
						console.log(e);
					}
				);
			}
			,async getFs(){
				var vm = this;
				send(
					'http://127.0.0.1/json/get/FS/*/*'
					,'GET'
					,''
					,function(msg){
						try{
							vm.drives = JSON.parse(msg);
						} catch(e) {
							console.log(e);
						}
					}
					,function(e){
						console.log(e);
					}
				);
			}
			,async getFiles(p,drive){
				var vm = this;
				vm.files[p]=[];
				send(
					'http://127.0.0.1/dir/'+drive+'/folder/*'
					,'GET'
					,''
					,function(msg){
						try{
							vm.files[p] = JSON.parse(msg);
							console.log(vm.files[p],p,vm.files);
						} catch(e) {
							console.log(e);
						}
					}
					,function(e){
						console.log(e);
					}
				);
			}
			,async check(test){
				var timer = setTimeout(function() { test.result='Сервер не ответил в течение 1 с.';test.isSucces=false; }, 1000);
				if(test.protocol===null) clearTimeout(timer);
				else if(test.protocol==='http'){
					send(
						test.url
						,test.method
						,test.data
						,function(msg){
							try{
								console.log(msg);
								var json = JSON.parse(msg);
								test.result='Проверка прошла успешно';
								test.isSucces=true;
								clearTimeout(timer);
							} catch(e) {
								console.log(e);
								test.result='Ошибка! '+e;
								test.isSucces=false;
								clearTimeout(timer);
							}
						}
						,function(e){
							console.log(e);
							test.result='Ошибка! '+e;
						}
					);
				}
				else if(test.protocol==='mqtt'){
					var client  = mqtt.connect('http://127.0.0.1:15675/ws');
					client.on('connect', function () {
						console.log('connected to MQTT brocker');
						client.subscribe('+'+test.topic, function (e) {
							if (!e) {
							  client.publish(test.topic, test.message);
							}
							else{
								console.log('MQTT subscribe error',e);
								test.result='Ошибка! '+e;
								test.isSucces=false;
							}
						});
					});
					client.on('message', function (topic, message) {
						if(topic.includes('sensor'+test.topic)){
							console.log(topic,message.toString());
							clearTimeout(timer);
							client.end();
							try{
								var json = JSON.parse(message.toString());
								test.result='Проверка прошла успешно';
								test.isSucces=true;
							} catch(e) {
								test.result='Ошибка! '+e;
								test.isSucces=false;
							}
							
						}
					});
					client.on('error', function (e) {
						console.log('WS MQTT error',e);
						test.result='Ошибка! '+e;
						test.isSucces=false;
						clearTimeout(timer);
						client.end()
					});
				}
			}
			,sectionStyle(isSucces){
				if(isSucces===null) return {'background-color': 'white'}
				if(isSucces) return {'background-color': '#37DE6A'}
				return {'background-color': '#FF5A40'}
			}
		}
		/*,computed: {}*/
	});
};

async function send(url,method,data,success, error, progress){
	console.log(method,url,data);		
	
	//var form = new FormData(document.forms.namedItem(data));
	var form = new FormData();
	if(data!==null)
		form.append('file', data, data.name);
	
	var xhttp = new XMLHttpRequest();			
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200){
			if (typeof success !== 'undefined') {
				try{
					success(this.responseText);
				} catch(e) {
					console.log(e);
				}
			}
		}
		else if (this.readyState == 4){
			console.log(this.responseText);
			if (typeof error !== 'undefined'){
				try{
					error(this.responseText);
				} catch(e) {
					console.log(e);
				}
			}
		}
	};		
	
	xhttp.upload.onprogress = function(event) {
		if (typeof progress !== 'undefined') {
			try{
				progress(Math.floor(event.loaded/event.total*100));
			} catch(e) {
				console.log(e);
			}
		}
	}			
	
	xhttp.open(method, url, true);
	//xhttp.send(data);
	if(data!==null)
		xhttp.send(form);
	else 
		xhttp.send();
}

