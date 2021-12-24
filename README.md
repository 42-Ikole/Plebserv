<p align="center">
  <img alt="Real cool image" src="https://github.com/K1ngmar/Plebserv/blob/main/plebserv.png" />
</p>

# Plebserv 🖥
A shitty webserver written by plebians [no plagiarism, no pet @rpet91]

## ABOUT 📝
We created a really cool HTTP 1.1 compliant webserver written in C++98

## FEATURES ✨
* [METHODS]
  * `GET`
  * `POST`
  * `DELETE`
  * `PUT`
  * `OPTIONS`
  * `HEAD`
* [CGI]
  * `Any cgi specified in the config file`
* [PORTS]
  * `listen on any port specified`
* [ERROR PAGES]
  * `setup custom default error pages`
* [LIMIT CLIENT BODY SIZE]
  * `upload limit for client`
* [HTTP REDIRECTION]
  * `custom http redirections`
* [AUTO INDEX]
  * `turn on directory listing`
* [SESSIONS and COOKIES]
  * `session management`

## CONFIG OPTIONS 📌
* [listen]
  * `listen domain:port port;`
* [server_name]
  * `server_name www.domain.nl doamain.nl;`
* [error_page]
  * `error_page [error_code] [location];`
* [location]
  * [autoindex]
    * `autoindex off;`
  * [root]
    * `root [location];`
  * [limit_except]
   * `limit_except [METHOD];`
  * [index]
    * `index [location1] [location2];`
  * [cgi]
    * `cgi [extension] [location];`
  * [client_max_body_size]
    * `client_max_body_size [uint][mod]`

## CONFIG EXAMPLE 📜

```c
server {
  listen phantom.nl:8080 400 5000;

  server_name www.phantom.nl phantom.nl;
	
  error_page 404 /err/404.html;
  error_page 500 /err/rip.html;

  location / {
    client_max_body_size 12k;
    autoindex off;
    index index.php index.html index.htm Welcome.html;
  }

  location /dev/ {
    autoindex off;
    root /www/;
    limit_except GET;
    index index.html;
    cgi .php /usr/local/bin/php-cgi;
  }

  location /HOME {
    autoindex on;
    limit_except GET HEAD POST DELETE OPTIONS TRACE PATCH;
    client_max_body_size 10m;
    index plebserv.php;
  }
}
```

## COLLAB WITH 💞
* [@Alpha_1337k](https://github.com/Alpha1337k)
* [@VictorTennekes](https://github.com/VictorTennekes)

### Special Shout Out 💯
* [@rpet91](https://github.com/rpet91)
* [@pde-bakk](https://github.com/pde-bakk)
* [@pikachu](https://i.kym-cdn.com/entries/icons/original/000/027/475/Screen_Shot_2018-10-25_at_11.02.15_AM.png)
  
# Confusion Counter ⁉️
  3
