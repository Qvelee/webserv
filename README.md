# webserv

Implementation of an HTTP-server in accordance with the rfc 7230-31 standard, supports cgi requests

## Usage

``make`` create ``webserv``  
``./webserv [path_to_config_file]`` starts the server. If the path is not specified, ``./webserver.conf`` is used

## Configuration file
Required settings:  
```
server {
  listen 127.0.0.1:8080;
  
  location / {
    method GET;
  }
}
```
* ``listen ip:port``  
* ``location path`` (path relative to the folder from which the program is launched)  

Optional settings:
```
server {
  listen 127.0.0.1:8080;
  server_name example.com;
  
  error_page 404 files/404.html;
  client_max_body_size 10;
  cgi .php /usr/bin/php-cgi;
  
  location / {
    method GET;
    method POST;
    alias files/;
    autoindex on;
    upload_store downloads/;
    index index.html;
  }
}

server {
  listen 127.0.0.1:8080;
  server_name example2.com;
  
  location /files {
    return 302 http://localhost:8080/;
  }
}  
```
* ``serve_name`` the name by which the server is selected if the ip matches
* ``error_page number path`` the page to be displayed on error, relative path
* ``client_max_body_size`` maximum request body size
* ``cgi file_extension absolute_path`` files to be processed by cgi
* ``method`` get post delete supported
* ``alias`` when searching on the server / will be replaced with files /
* ``autoindex`` turn on or off directory listing
* ``upload_store`` directory where files will be saved when post request
* ``index`` default file to answer if the request is a directory
* ``return status_code url`` redirection
