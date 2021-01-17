

include_directories("./include")
add_library(llhttp STATIC "./src/api.c" 
                          "./src/http.c"
                          "./src/llhttp.c")