file (GLOB bsoncxx-document_headers *.hpp)
file (GLOB bsoncxx-document_sources *.cpp)

add_library(bsoncxx-document OBJECT ${bsoncxx-document_sources})
