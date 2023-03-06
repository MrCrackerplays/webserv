#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP
# include <string>

class AutoindexGenerationException : public std::exception {
	public:
		const char* what() const throw();
};

std::string generate_autoindex(std::string path, std::string display_path);

#endif
