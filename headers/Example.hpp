#ifndef EXAMPLE_HPP
# define EXAMPLE_HPP
/*Includes*/
class Example {
protected:
	/*Attributes*/
	int	value;
public:
	Example(/*Args*/);
	Example(Example const & src);

	~Example(void);

	Example & operator=(Example const & rhs);

	/*Member Functions*/
};
#endif
