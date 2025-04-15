# From the subject

__Summary:__  
This project is about writing your own HTTP server.  
Version: 21.4  

You must write an HTTP server in C++ 98.  
• Your program must take a configuration file as an argument, or use a default path.  
• Your server must remain non-blocking at all times and properly handle client disconnections when necessary.  
• It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O
operations between the client and the server (listen included).  
• poll() (or equivalent) must monitor both reading and writing simultaneously.  
• You must never do a read or a write operation without going through poll() (or
equivalent).  
• Checking the value of errno is strictly forbidden after performing a read or write
operation.  
• A request to your server should never hang indefinitely.  
• Your HTTP response status codes must be accurate.  
• Your server must have default error pages if none are provided.  
• You can’t use fork for anything other than CGI (like PHP, or Python, and so forth).  
• You must be able to serve a fully static website.  
• Clients must be able to upload files.  
• You need at least the GET, POST, and DELETE methods.  

Configuration file:  
• Choose the port and host of each ’server’.  
• Set up the server_names or not.  
• The first server for a host:port will be the default for this host:port (meaning it
will respond to all requests that do not belong to another server).  
• Set up default error pages.  
• Set the maximum allowed size for client request bodies.  
• Set up routes with one or multiple of the following rules/configurations (routes
won’t be using regexp):
◦ Define a list of accepted HTTP methods for the route.  
◦ Define an HTTP redirect.  
◦ Define a directory or file where the requested file should be located (e.g.,
if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
/tmp/www/pouic/toto/pouet).  
◦ Enable or disable directory listing.  
◦ Set a default file to serve when the request is for a directory.  
◦ Execute CGI based on certain file extension (for example .php).  
◦ Make it work with POST and GET methods.  
◦ Allow the route to accept uploaded files and configure where they should be
saved.  

__Bonus__  

• Support cookies and session management.  
• Handle multiple CGI.  

# Usage

`make`, `./webserv [conf file]`.  

Configuration file:  
-`server`: create a server  
-`location [route name]`: create a route with a name (`http://web.site/route/wow` could match route `/route`)  
-`listen [ip]/[port]/[ip:port]`: set ip and port for the server socket  
-`server_name [name] ...`: set server name(s), used to choose which server to use when ip and port are the same 
(`http://web.site/route/wow` will only match `web.site`)  
-`error_page [code] ... [error page]`: set custom error pages for one or multiple error codes  
-`client_max_body_size [size](k/m/g)`: set max allowed body size for requests. 0 means unlimited  
-`root [root]`: set where a route will be mounted (`/route/wow` becomes `[root]/route/wow`)  
-`alias [alias]`: set where a route will be mounted, replacing the route name 
(`/route/wow` becomes `[alias]/wow`)  
-`methods [METHOD] ...`: set allowed methods for the route  
-`index [file] ...`: set index files (`/route/` will search for `/route/[file]`)  
-`autoindex [on/off]`: enable or disable autoindex  
-`cgi [extension] [binary]`: set file extensions to be executed with the binary  
-`return http(s)://[website]`: set a redirection for the route   
-`upload [upload directory]`: allow uploads and set a directory  
See examples in `conf/`.
