  /*
  
c	Match the literal character c once, unless it is one of the special characters.
^	Match the beginning of a line.
.	Match any character that isn't a newline.
$	Match the end of a line.
|	Logical OR between expressions.
()	Group subexpressions.
[]	Define a character class.
*	Match the preceding expression zero or more times.
+	Match the preceding expression one ore more times.
?	Match the preceding expression zero or one time.
{n}	Match the preceding expression n times.
{n,}	Match the preceding expression at least n times.
{n, m}	Match the preceding expression at least n times and at most m times.
\d	Match a digit.
\D	Match a character that is not a digit.
\w	Match an alpha character, including the underscore.
\W	Match a character that is not an alpha character.
\s	Match a whitespace character (any of \t, \n, \r, or \f).
\S	Match a non-whitespace character.
\t	Tab.
\n	Newline.
\r	Carriage return.
\f	Form feed.
\m	Escape m, where m is one of the metacharacters described above: ^, ., $, |, (), [], *, +, ?, \, or /.

#include <boost/regex.hpp>

  std::string s, sre;
   boost::regex re;
  while(true)
   {
      cout << "Expression: ";
      cin >> sre;
      if (sre == "quit")
      {
         break;
      }
      cout << "String:     ";
      cin >> s;

      try
      {
         // Set up the regular expression for case-insensitivity
         re.assign(sre, boost::regex_constants::icase);
      }
      catch (boost::regex_error& e)
      {
         cout << sre << " is not a valid regular expression: \""
              << e.what() << "\"" << endl;
         continue;
      }
      if (boost::regex_match(s, re))
      {
         cout << re << " matches " << s << endl;
      }else
	{
		cout << re << " does not match " << s << endl;
	};
   }
   */

	/*
	std::string s, sre;
	boost::regex re;
	sre="[-\\w\\.]+obj";
	s = "ab-2-4T_baa.obj";
	*/
	wchar_t* sre=L"[-\\w\\.]+obj";
	wchar_t* s=L"ab-2-4T_baa.obj";
	boost::wregex re;

	try
	{
		// Set up the regular expression for case-insensitivity
		re.assign(sre, boost::regex_constants::icase);
	} catch (boost::regex_error& e)
	{
		wcout << sre << " is not a valid regular expression: \"" << e.what() << "\"" << endl;
	}
	if (boost::regex_match(s, re))
	{
		wcout << re << " matches " << s << endl;
	} else
	{
		wcout << re << " does not match " << s << endl;
	};

