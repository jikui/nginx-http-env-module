# nginx-http-env-module

examples:

env NEW_VARIABLE;

env UPSTREAM;

http {

    log_format env_format "... $env_NEW_VARIABLE ";
    
    server {
    
        listen 8080;
        
        location / {
        
            proxy_pass http://$env_UPSTREAM;
            
        }
        
    }
    
}
