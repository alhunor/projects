#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Alternative dictionaries and thesaurus
// https://wiki.openoffice.org/wiki/Dictionaries

// documnted under: https://wordnet.princeton.edu/wordnet/man/wndb.5WN.html

typedef char synsetType;

struct lexicographerT
{
	synsetType ss_type; // can be:  n    NOUN, v    VERB, a    ADJECTIVE, s    ADJECTIVE SATELLITE, r    ADVERB
	std::string category;
	std::string explanation;
};

std::vector<lexicographerT> lex_files;

std::ostream& operator<<(std::ostream& os, lexicographerT& lg)
{
	os << lg.ss_type << " " << lg.category << " -> " << lg.explanation;
	return os;
}

bool read_lexicographer(const char* fileName)
{
	char s[1024];
	std::fstream f;
	f.open(fileName);
	lexicographerT lg;
	while (!f.eof())
	{
		f.getline(s, 1024);
		/*
		format of s is dd syntacticCategory.category explanation, e.g.
			02 adv.all all adverbs
			05 noun.animal nouns denoting animals
			15 noun.location nouns denoting spatial position
		*/
		int id = atoi(s);
		int i, j;
		switch (s[5])
		{
		case 'u':
			lg.ss_type = 'n'; i = 8;  break;
		case 'r':
			lg.ss_type = 'v'; i = 8; break;
		case 'j':
			lg.ss_type = 'a'; i = 7; break;
		case 'v':
			lg.ss_type = 'r'; i = 7; break;
		default: return false;
		}
		// "i" point to the start of the "category"
		// look for the next space
		for (j = i; s[j] != ' '; ++j);
		s[j] = 0;
		lg.category = s + i;
		lg.explanation = s + j+1;
		lex_files.push_back(lg);
		std::cout << id << "  " <<lg << std::endl;
	}
	return true;
} // bool read_lexicographer(const char* fileName)


struct PTR
{
	std::string pointer_symbol;
	/* Pointer symbols for nouns can be :
	!	Antonym
	@   Hypernym
	@i  Instance Hypernym
	~	Hyponym
	~i  Instance Hyponym
	#m  Member holonym
	#s  Substance holonym
	#p  Part holonym
	%m  Member meronym
	%s  Substance meronym
	%p  Part meronym
	=   Attribute
	+   Derivationally related form
	;c  Domain of synset - TOPIC
	-c  Member of this domain - TOPIC
	;r  Domain of synset - REGION
	-r  Member of this domain - REGION
	;u  Domain of synset - USAGE
	-u  Member of this domain - USAGE
	*/
	/* Pointer symbols for adjectives can be :
	!Antonym
	&    Similar to
	<    Participle of verb
	\    Pertainym(pertains to noun)
	= Attribute
	^    Also see
	;c    Domain of synset - TOPIC
	;r    Domain of synset - REGION
	;u    Domain of synset - USAGE
	*/

	/* Pointer symbols for verbs can be :
	!    Antonym
	@    Hypernym
	 ~    Hyponym
	 *    Entailment
	 >    Cause
	 ^    Also see
	 $    Verb Group
	 +    Derivationally related form        
	 ;c    Domain of synset - TOPIC
	 ;r    Domain of synset - REGION
	 ;u    Domain of synset - USAGE
	*/
	/* Pointer symbols for adverbs can be :
	!	Antonym
	\    Derived from adjective
	;c    Domain of synset - TOPIC
	;r    Domain of synset - REGION
	;u    Domain of synset - USAGE
	*/
	int synset_offset;
	synsetType pos;
	char source, target;
	bool check_pointer_symbol();
};

bool PTR::check_pointer_symbol()
{
	int len = pointer_symbol.length();

	switch (pos)
	{
	case 'n': //noun
		switch (pointer_symbol[0])
		{
		case '!':
		case '=':
		case '+':
		case '\\': //added by Hunor
			return (len == 1);
		case '@':
		case '~':
			return (len == 1) || pointer_symbol[1] == 'i';
		case '#':
		case '%':
			return (len == 2) && (pointer_symbol[1] == 'm' || pointer_symbol[1] == 's' || pointer_symbol[1] == 'p');
		case ';':
		case '-':
			return (len == 2) && (pointer_symbol[1] == 'u' || pointer_symbol[1] == 'r' || pointer_symbol[1] == 'c');
		}
		break;
	case 'a': //adjective
		switch (pointer_symbol[0])
		{
		case '!':
		case '&':
		case '<':
		case '\\':
		case '=':
		case '+': // not in spec, added by Hunor +Derivationally related form
		case '^':
			return (len == 1);
		case '-': // added by Hunor
		case ';':
			return (len == 2) && (pointer_symbol[1] == 'u' || pointer_symbol[1] == 'r' || pointer_symbol[1] == 'c');
		}
		break;
	case 'v': // verbs
		switch (pointer_symbol[0])
		{
		case '!':
		case '@':
		case '~':
		case '*':
		case '=':
		case '<': // // added by Hunor
		case '>':
		case '^':
		case '$':
		case '+':
			return (len == 1);
		case '-': // added by Hunor
		case ';':
			return (len == 2) && (pointer_symbol[1] == 'u' || pointer_symbol[1] == 'r' || pointer_symbol[1] == 'c');
		}
		break;
	case 'r': // adverbs
		switch (pointer_symbol[0])
		{
		case '+': // added by Hunor
		case '!':
		case '\\':
			return (len == 1);
		case '-': // added by Hunor
		case ';':
			return (len == 2) && (pointer_symbol[1] == 'u' || pointer_symbol[1] == 'r' || pointer_symbol[1] == 'c');
		}
		break;

	default:
		return false;
	}

	return false;
}

struct synsetT
{
	int synset_offset; // Current byte offset in the file represented as an 8 digit decimal integer. 
	int lex_filenum; // Two digit decimal integer corresponding to the lexicographer file name containing the synset.
	synsetType ss_type; // One character code indicating the synset type: n,v,a,r,s
	char w_cnt;// Two digit hexadecimal integer indicating the number of words in the synset.
	std::vector<std::string>  words;
	int p_cnt; // Three digit decimal integer indicating the number of pointers from this synset to other synsets. If p_cnt is 000 the synset has no pointers
	std::vector<PTR>  pointers; // A pointer from this synset to another.ptr is of the form : pointer_symbol  synset_offset  pos  source / target
	std::string gloss;
};

std::vector<synsetT> synsets_n, synsets_v, synsets_a, synsets_r;

char hexdigit(char a)
{
	if (a > 47 && a < 58) return a - 48;
	if (a > '@' && a < 'G') return a - 55;
	if (a > '`' && a < 'g') return a - 87;
	throw "bad digit";
}


bool read_data(const char* fileName, char pos)
{
	// format is described in http://wordnet.princeton.edu/wordnet/man/wndb.5WN.html
	const int MAX_LEN = 18000;
	char s[MAX_LEN];
	std::fstream f;
	f.open(fileName);
	synsetT ss;
	int i, j, start;
	int nb = 0;
	
	std::vector<synsetT>* synsets;
	switch (pos) // part of speach
	{
		case 'n':
			synsets = &synsets_n;
			break;
		case 'v':
			synsets = &synsets_v;
			break;
		case 'a':
			synsets = &synsets_a;
			break;
		case 'r':
			synsets = &synsets_r;
			break;
		default:
			return false;
	}
	

	std::cout << " Processing " << fileName << " : ";

	while (!f.eof())
	{
		f.getline(s, MAX_LEN);
		int gc = (int)f.gcount();
		if (gc == 0) break; // reached the end of the file
		if ( gc == MAX_LEN-1) // line is too long
		{
			std::cout << "increase len." << std::endl;
			throw "increase len.";
		}
		if (s[0] == ' ') continue; // skip licence section

		ss.synset_offset = atoi(s);
		//std::cout << ss.synset_offset << " ";
		if (ss.synset_offset == 14877)
		{
 			ss.synset_offset = 14877;
		}
		ss.lex_filenum = atoi(s+9);
		ss.ss_type = s[12];
		ss.w_cnt = hexdigit(s[14])*16+ hexdigit(s[15]);
		j = 17;
		// get synset
		ss.words.resize(ss.w_cnt);
		for (i = 0; i < ss.w_cnt; ++i)
		{
			// read word and lex_id
			start = j;
			while (s[j] != ' ') ++j;
			s[j] = 0; ++j;
			std::string word = s + start;
			int lex_id = atoi(s + j);
			if (lex_id !=0)
			{
				word += lex_id + 48;
			}
			if (lex_id >1)
			{
				lex_id++;
			}
			j+=2;
			ss.words.push_back(word);
		}
		ss.p_cnt = atoi(s + j); // number of pointers
		ss.pointers.resize(ss.p_cnt);
		// get pointers
		j += 4;
		PTR ptr; 
		for (i = 0; i < ss.p_cnt; ++i)
		{
			// read pointer symbol
			ptr.pointer_symbol = s[j];
			if (s[j + 1] != ' ')
			{
				++j;
				ptr.pointer_symbol += s[j];
			}
			ptr.synset_offset = atoi(s + j + 2);
			j += 11;

			ptr.pos = s[j];
			j+=2;

			if (!ptr.check_pointer_symbol())
			{
				throw "File format error";
			}

			ptr.source = hexdigit(s[j]) * 16 + hexdigit(s[j + 1]);
			j += 2;
			ptr.target = hexdigit(s[j]) * 16 + hexdigit(s[j + 1]);
			j += 3;
			ss.pointers.push_back(ptr);
		}
		// read gloss, i.e. everything after "|" character
		ss.gloss = s + j + 2;
		synsets->push_back(ss); ++nb;
	}
	std::cout << nb << " definitions read." << std::endl;

	return true;
} // bool read_data(const char* fileName)

struct lemmaT
{
	std::string word;
	synsetType ss_type; // 
	int synset_cnt; //Number of synsets that lemma is in. This is the number of senses of the word in WordNet.
	int p_cnt; //	Number of different pointers that lemma has in all synsets containing it.
	std::string ptr_symbol; // A space separated list of p_cnt different types of pointers that lemma has in all synsets containing itIf all senses of lemma have no pointers, this field is omitted and p_cnt is 0 .
	std::vector<int> synset_offset; // Byte offset in data.pos file of a synset containing lemma.
	bool operator<(lemmaT& lhs) { return word < lhs.word; }
};

std::vector<lemmaT> index_n, index_v, index_a, index_r;
std::map<int, int> translation_n, translation_v, translation_a, translation_r;

bool read_index(const char* fileName, char pos)
{
	
	const int MAX_LEN = 600;
	char s[MAX_LEN];
	std::fstream f;
	f.open(fileName);
//	int i, j, start;
	int nb = 0;
	std::cout << " Processing " << fileName << " : ";

	std::vector<lemmaT> *index;
	switch (pos) // part of speach
	{
	case 'n':
		index = &index_n;
		break;
	case 'v':
		index = &index_v;
		break;
	case 'a':
		index = &index_a;
		break;
	case 'r':
		index = &index_r;
		break;
	default:
		return false;
	}
	while (!f.eof())
	{
		f.getline(s, MAX_LEN);
		int gc = (int)f.gcount();
		if (gc == 0) break; // reached the end of the file
		if (gc == MAX_LEN - 1) // line is too long
		{
			std::cout << "increase len." << std::endl;
			throw "increase len.";
		}
		if (s[0] == ' ') continue; // skip licence section
		int i, start;
		int j = 0;
		while (s[j] != ' ') ++j;
		s[j] = 0;
		++j;
		lemmaT lem;
		lem.word = s;
		//std::cout << lem.word << " ";
		lem.ss_type = s[j];
		j+=2;
		lem.synset_cnt = atoi(s + j);
		while (s[j] != ' ') ++j; j++;
		lem.p_cnt = atoi(s + j);
		while (s[j] != ' ') ++j; ++j;
		// consume pointer string
		i = 0;
		start = j;
		while (i < lem.p_cnt)
		{
			while (s[j] != ' ') ++j; 
			++i;
			++j;
		}
		s[j] = 0;
		lem.ptr_symbol = s + start;
		// skip next number as it is redundant
		while (s[j] != ' ') ++j; j++;
		// skip tagsense_cnt too
		while (s[j] != ' ') ++j; j++;
		// read offsets
		for (i = 0; i < lem.synset_cnt; ++i)
		{
			int offset = atoi(s+j);
			lem.synset_offset.push_back(offset);
			while (s[j] != ' ') ++j; j++;
		}

		index->push_back(lem);
		++nb;
	} // while (!f.eof())
	std::cout << nb << " definitions read." << std::endl;

	return true;
}

bool postprocessing(std::vector<lemmaT>& index, std::vector<synsetT>& synsets, std::map<int, int>& translation)
{
	unsigned int i, j;
	for (i = 0; i < synsets.size(); ++i)
	{
		j = synsets[i].synset_offset;
		translation[j] = i;
	}
	for (i = 0; i < index.size(); ++i)
	{
		std::vector<int>& v = index[i].synset_offset;
		for (j = 0; j < v.size(); ++j)
		{
			v[j] = translation[v[j]];
		}
	}
	return true;
} // bool postprocessing(char pos)



bool reindexing(std::vector<synsetT>& synsets)
{
	unsigned int i, j;
	std::map<int, int>* translation;

	for (i = 0; i < synsets.size(); ++i)
	{
		std::vector<PTR>& p = synsets[i].pointers;
		for (j = 0; j < p.size(); ++j)
		{
			switch (p[j].pos) // part of speach
			{
			case 'n':
				translation = &translation_n;
				break;
			case 'v':
				translation = &translation_v;
				break;
			case 'a':
				translation = &translation_a;
				break;
			case 'r':
				translation = &translation_r;
				break;
			default:
				return false;
			} // switch (pos) // part of speach	
			p[j].synset_offset = (*translation)[p[j].synset_offset];
		}
	}
	return true;
} // bool postprocessing(char pos)


bool find_word(char* s, std::vector<lemmaT>& index)
{
	binsearch();
}


void find_word(char* s)
{
	find_word(s, index_n);
}


void main()
{
	read_lexicographer("lexicographer.txt");
	read_data("dict\\data.noun", 'n');
	read_data("dict\\data.verb", 'v');
	read_data("dict\\data.adj", 'a');
	read_data("dict\\data.adv", 'r');

	read_index("dict\\index.adv", 'r');
	read_index("dict\\index.verb", 'v');
	read_index("dict\\index.noun", 'n');
	read_index("dict\\index.adj", 'a');

	// GET ADDRESSES OF ALL OFFSETS
	postprocessing(index_n, synsets_n, translation_n);
	postprocessing(index_a, synsets_a, translation_a);
	postprocessing(index_r, synsets_r, translation_r);
	postprocessing(index_v, synsets_v, translation_v);
	// REPLACE POINTERS WITH THE OFFSETS
	reindexing(synsets_n);
	reindexing(synsets_v);
	reindexing(synsets_a);
	reindexing(synsets_r);

	find_word("sheep");

	std::cout << "Press any key to exit..." << std::endl;
	_getch();
}