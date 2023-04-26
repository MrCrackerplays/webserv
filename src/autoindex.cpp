#include <sstream>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>
#include "autoindex.hpp"

const char* AutoindexGenerationException::what() const throw() {
	return ("Error generating autoindex!");
}

static void generate_table(std::ostringstream & index, std::string & path, std::string & display_path) {
	DIR *	dir = opendir(path.c_str());
	if (!dir)
		throw AutoindexGenerationException();
	struct dirent *	ent = readdir(dir);
	while (ent != NULL) {
		if (std::string(ent->d_name) != "." && (display_path != "/" || std::string(ent->d_name) != "..")) {
		index << "<tr>";
		index << "<td>";
		index << "<a href=\"" << display_path;
		if (display_path[display_path.size() - 1] != '/')
			index << "/";
		index << ent->d_name << "\">" << ent->d_name << "</a>";
		{
			struct stat	attributes;
			if (stat((path + "/" + ent->d_name).c_str(), &attributes) != 0)
				throw AutoindexGenerationException();
			//file last modified
			char	date[20];
			tm *	time = localtime(&attributes.st_ctime);
			if (!time)
				throw AutoindexGenerationException();
			if (strftime(date, 20, "%d-%b-%Y %H:%M", time) == 0)
				date[0] = '\0';
			index << "<td>" << date << "</td>";
			//file size
			index << "<td>" << attributes.st_size << "</td>";
		}
		index << "</td>";
		index << "</tr>";
		}
		ent = readdir(dir);
	}
	closedir(dir);
}

std::string generate_autoindex(std::string path, std::string display_path)
{
	std::ostringstream	index;
	index << "<html>";
	index << "<head>";
	index << "<title>Index of " + display_path + "</title>";
	index << "<style>";
	index << "table{font-family:monospace; margin-top: 1em;}";
	index << "td {padding-left: 1em;}";
	index << "td:first-child {padding-left:0}";
	index << "</style>";
	index << "</head>";
	index << "<body>";
	index << "<h1>Index of " + display_path + "</h1>";
	index << "<hr>";
	index << "<table>";
	generate_table(index, path, display_path);
	index << "</table>";
	index << "<hr>";
	index << "</body>";
	index << "</html>";
	return (index.str());
}