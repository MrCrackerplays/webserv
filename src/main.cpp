#include <iostream>
#include <Server.hpp>

void	funk(Server &server_block) {
	Location some_location_block("/shii", "yesredirect", "root", false, "index.html", "thissetcookie", "thatrequirecookie", 50);
	some_location_block.setRequireCookie("HEYOOOO");
	std::cout << some_location_block << std::endl;
	std::cout << "path:" << some_location_block.getPath() << std::endl;
	// a.setRedirect("noredirect");
	server_block.addLocation(some_location_block.getPath(), some_location_block);
	std::cout << server_block << std::endl;
}

int main(void){
	Server	server_block(8080, "localhost", 9, "kane");
	server_block.addError("404", "yuh");
	funk(server_block);
	std::cout << server_block << std::endl;
	// std::cout << s.getErrorPage("500", "/shii/yuh/yuk") << std::endl;
}
