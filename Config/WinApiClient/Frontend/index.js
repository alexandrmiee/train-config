window.onload = function() {
	
	new Vue(
	{
		template: '#app-tmpl'
		,el:'#app'
		,created() {}
		,data(){
			return{
				clients:[]
				
			}
		}
		,components:{}
		,methods:{
			save(){}
			,start(i){}
			,deleteClient(i){
				this.clients.splice(i,1);
			}
			,addClient(){
				this.clients.push({
					port:8080
					,bet:'marafon.com'
					,isWork:false
				});
			}
		}
		,computed: {}
	});
	
};


async function send(url,method,data,success, error, progress){
	console.log(method,url,data);		
	var fd = new FormData();
	fd.append('json',JSON.stringify(data));
	
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
	// xhttp.send(fd);
}

