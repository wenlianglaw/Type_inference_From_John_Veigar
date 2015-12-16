#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <algorithm> // sort
#include <set>

using namespace std;

#define SHOW_DEBUG_INFO 1
#if SHOW_DEBUG_INFO == 0
#define printf printf_
void printf_(...) {/* do nothing */}
#endif

/*

<TYPE> ::= <TYPEVAR> | <PRIMITIVE_TYPE> | <FUNCTYPE> | <LISTTYPE>;
<PRIMITIVE_TYPE> ::= ¡®int¡¯ | ¡®real¡¯ | ¡®str¡¯;
<TYPEVAR> ::= ¡®`¡¯ <VARNAME>; // Note, the character is a backwards apostrophe!
<VARNAME> ::= [a-zA-Z][a-zA-Z0-9]*; // Initial letter, then can have numbers
<FUNCTYPE> ::= ¡®(¡® <ARGLIST> ¡®)¡¯ -> <TYPE> | ¡®(¡® ¡®)¡¯ -> <TYPE>;
<ARGLIST> ::= <TYPE> ¡®,¡¯ <ARGLIST> | <TYPE>;
<LISTTYPE> ::= ¡®[¡® <TYPE> ¡®]¡¯;


VALID_INPUT ::= QUERY_LIST ¡®QUIT¡¯ ¡®\n¡¯
QUERY_LIST ::= UNIFICATION_QUERY QUERY_LIST | 


LwL
2015-12-11
NYU
*/


class Type;


typedef unsigned int	id;



/**********************************************
				Unify Algorithm:
Class:
	Type			: Basic information of a type
	Environment		: the types' environment

unify( type1, type2 , environment ) :-
	mgt1, mgt2 := most_general_type of type1, type2
	if mgt1 == mgt2:
		return 1
	Case mgt1, mgt2:
		1 NIL and other is not NIL: ( (type1==NIL) ^  (type2==NIL) )
			"BOTTOM"
		1+ Var:
			if mgt1 in mgt2 OR mgt2 in mgt1		// cause recursive call
				"BOTTOM"
			env._union( mgt1, mgt2 )
		Primitive:
			if( mgt1 != mgt2 )
				"BOTTOM"
			else
				Do nothing
		Functype:
			if( arg_list1.size() != arg_list2.size() )
				"BOTTOM"
			else
				for_each element in arg_list:
					unify ( ele1, ele2 )
				unify ( ret1, ret2 )
		Listtype: 
			mgt@( [ inner_part ] )
			unify ( inner_part1 ,  inner_part2 ) 
			env.union_ ( mgt1, mgt2 )
	end

most_general( type,rst,env ) :-
	case type:
		Functype:
			buff += "("
			for_each ai in arglist:
				most_general( ai, rst, env )
				buff += ","
			buff.pop_back(); buff += ") ->"
			most_general( ret, buff, env )
		Var:
			if trace( type, buff2, env ) success
				buff += buff2
			else
				"BOTTOM"
		Primitive:
			buff += type.str
		Listtype:
			buff += "["
			most_general( inner_type, buff, env )
			buff += "]"
	end

trace( type, buff, env , visit)
	root := env.root(type)
	if type in visit:
		// its a loop
		"BOTTOM"
	else
		visit.insert(type)
		env.sort_sets(root)		// sets[root][0] is the type that we want
		if sets[root][0] is ( PRIMITIVE ||  VAR )
			buff += sets[root][0]
		else
			// continue trace
			mgt := most_general( sets[root][0] , buff2, env )
			buff += buff2
			// update the set
			env.union_( root, mgt )
	end
**********************************************/

/**********************************************
				Parser
Functions:
	type
	arg_list
	func_type
	type_var
	list_type

int type(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	int ret = func_type(str, buff, env) || type_var(str, buff,env) || primitive_type(str, buff,env) || list_type(str, buff, env);
	if (!ret) {
		str = ori;
	}
	return ret;	
}

func_type ( str, buff, env )
	original_str := str
	ret = Type::FUNCTYPE
	
	if( eatChar( '(' ) && arg_list() && eatChar( ')' ) && eatPrefix( "->" ) && type() )
		buff += "(" + arg_list + ") ->" + type
	else if( eatChar( '(' ) && eatChar( ')' ) && eatPrefix( "->" ) && type() )
		buff += "() ->" + type
		// add "" as a new type
		env.addType( "" )
	else
		ret := 0
	if( ret != 0 )
		// add this functype
		env.addType( buff )
	return ret
	end

int arg_list(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	int ret = 1;
	string type_buff;
	if (type(str, type_buff, env)) 
		string ori2 = str;
		if (eatChar(str, ',') && arg_list(str, buff, env)) 
			buff = type_buff + "," + buff;
		else 
			str = ori2;
			buff += type_buff;
	else
		str = ori;
		ret = 0;
	return ret;
}
...
**********************************************/

/**********************************************
				Classes
Environment:
public:
	vector<id> m_v_Root;
	vector<id> m_v_Rank;
	// e.g. sets[1] stores all elements that has same type of 1
	vector<vector<id>> m_v_sets;
	// All types stored here
	vector<Type> vT;
private:
	// all the registed types are stored here
	map<string, id> m_indexType;
	id m_id;

Functions:
qsort t1,t2
	primitive_num DESC
	if pri1 == pri2
		var_num DESC
		if var1 == var2
			[] > ![]

Disjoint Set
	union()
	root()

id_generator()
	return m_id++

int add_type(const Type &type) {
	vT.push_back(type);
	m_indexType.insert(pair<string, id>(type.m_str, type.m_type_id));
	m_v_Root.push_back(type.m_type_id);
	m_v_Rank.push_back(1);
	m_v_sets.push_back(vector<id>(1,type.m_type_id));
	return 1;
}
**********************************************/
class Type {
public:
	static const int VAR		= 1;
	static const int PRIMITIVE	= 2;
	static const int FUNCTYPE = 4;
	static const int LISTTYPE = 8;
	static const int NIL = 16;
public:
	id m_type_id;
	vector<id> m_arg;
	id m_ret;
	string m_str;
	int m_num_primitive;
	int m_num_vars;
	int type;

	bool operator <(const Type& rh)const {
		return m_num_primitive < rh.m_num_primitive;
	}
	Type(id _id, string _str = "",int _type = 0, int _num =0,int _num_var=0,  id _ret =0, id _arg = 0 )
		:m_type_id(_id),m_arg(_arg),m_ret(_ret),m_str(_str),m_num_primitive(_num),type(_type), m_num_vars(_num_var)
	{}

};

class Environment {
public:
	id id_generator() {
		return m_id++;
	}

	bool is_type_exists(const Type &type) const {
		return m_indexType.count(type.m_str);
	}
	bool is_type_exists(const string &str) const {
		return m_indexType.count(str);
	}

	id get_type_ID(const string& str) const {
		return m_indexType.at(str);
	}
	id get_type_ID(const Type &type) const {
		return m_indexType.at(type.m_str);
	}

	Type get_type(id _id)const {
		return vT[_id-1];
	}
	Type get_type(const string &str) {
		return vT[get_type_ID(str) - 1];
	}

	int add_type(const Type &type) {
		if (is_type_exists(type.m_str)) {
			system("pause");
		}
		vT.push_back(type);
		m_indexType.insert(pair<string, id>(type.m_str, type.m_type_id));
		m_v_Root.push_back(type.m_type_id);
		m_v_Rank.push_back(1);
		m_v_sets.push_back(vector<id>(1,type.m_type_id));
		return 1;
	}

	/**************************************************** 
			quick sort a vector by type index
	****************************************************/
	void sort_sets(int set_No) {
		struct Qsort {
			int p(int s, int e, int set_No, Environment &env) {
				int last_pos = s;
				int i = s;
				vector<id> &vect = env.m_v_sets[set_No];
				const Type &cmp_type = env.get_type(vect[e-1]);
				for (; i < e - 1 ; i++) {
					bool b_swap = false;
					const Type &type_i = env.get_type(vect[i]);
					if (type_i.m_num_primitive > cmp_type.m_num_primitive) {
						b_swap = true;
					}
					else if(type_i.m_num_primitive == cmp_type.m_num_primitive){
						// num_primitive1 == num_primitive2
						if (type_i.m_num_vars > cmp_type.m_num_vars) {
							b_swap = true;
						}
						else if (type_i.m_num_vars == cmp_type.m_num_vars) {
							if (type_i.type == Type::LISTTYPE && cmp_type.type == Type::VAR) {
								b_swap = true;
							} 
							else {
								b_swap = false;
							}
						} 
						else {
							b_swap = false;
						}
					}
					else {
						// pri1 < pri2
						b_swap = false;
					}
					if (b_swap) {
						swap(vect[last_pos], vect[i]);
						last_pos++;
					}
				}
				swap(vect[last_pos], vect[e - 1]);
				return last_pos;
			}
			void qs(int s, int e, int set_No, Environment &env) {
				if (s >= e) return;
				int n_p = p(s, e, set_No, env);
				qs(s, n_p, set_No, env);
				qs(n_p + 1,e , set_No, env);
			}
		};
		Qsort Q;
		Q.qs(0, m_v_sets[set_No].size(), set_No, *this);
	}

	/****************************************************
					Disjoint Set
	union
	root
	****************************************************/
	int union_(Type t1, Type t2) {
		int root1 = root(t1.m_type_id);
		int root2 = root(t2.m_type_id);
		if (root1 == root2) return 1;
		if (m_v_Rank[root1] > m_v_Rank[root2]) {
			m_v_Root[root2] = m_v_Root[root1];
			append(m_v_sets[root1], m_v_sets[root2]);
		} else {
			if (m_v_Rank[root1] == m_v_Rank[root2]) {
				m_v_Rank[root1] ++;
			}
			m_v_Root[root1] = m_v_Root[root2];
			append(m_v_sets[root2], m_v_sets[root1]);
		}
		return 1;
	}
	int union_(id _id1, id _id2) {
		return union_(get_type(_id1), get_type(_id2));
	}

	int root(id _id) {
		if (m_v_Root[_id] != _id) {
			m_v_Root[_id] = root(m_v_Root[_id]);
			return m_v_Root[_id];
		}else {
			return m_v_Root[_id];
		}
	}

	
public:
	vector<id> m_v_Root;
	vector<id> m_v_Rank;

	// e.g. sets[1] stores all elements that has same type of 1
	vector<vector<id>> m_v_sets;

	// All types stored here
	vector<Type> vT;


	Environment() :m_id(1) { m_v_Root.push_back(0); m_v_Rank.push_back(1); m_v_sets.push_back(vector<id>()); }
private:
	// all the registed types are stored here
	map<string, id> m_indexType;
	id m_id;

	void append(vector<id> &v, vector<id> &appd) {
		for (auto vi : appd) {
			v.push_back(vi);
		}
	}
};

class MyException {
public:
	string msg;
	MyException(string _str) :msg(_str)
	{}
	string what() { return msg; }
};


/***************************************************

Sub functions

***************************************************/
int type(string &str, string &buff, Environment &env);
int type_var(string &str, string &buff, Environment &env);
int primitive_type(string &str, string &buff, Environment &env);

/* return the valid name's length */
int valid_name(const string &name, int is_first_call = 1, int length = 0) {
	if (is_first_call) {
		if (isalpha(name[0])) {
			return valid_name(name.substr(1), 0,length+1);
		} else {
			return length;
		}
	} else {
		if (name.empty()) {
			return length;
		}else if (isalnum(name[0])) {
			return valid_name(name.substr(1), 0,length+1);
		} else {
			return length;
		}
	}
}

bool isPrefix(const char *str, const char *pre) {
	while(*pre && *pre++ == *str++);
	return !(*pre);
}
void eatSpace(string &str) {
	int i;
	for (i = 0; i < str.length() && str[i] == ' ' || str[i] == '\t';i++);
	str = str.substr(i);
}
bool eatChar(string &str,char ch) {
	string ori = str;
	eatSpace(str);
	if (str[0] == ch) {
		str = str.erase(0,1);
		return true;
	} else {
		return false;
	}
}
bool eatPrefix(string &str, char *pre) {
	eatSpace(str);
	bool loop = true;
	for (; loop && *pre; loop &= eatChar(str, *pre++));
	return loop;
}

set<string> extract_var_name(string &str, Environment &env, set<string> _s = set<string>()) {
	if (str.empty()) {
		return _s;
	} else {
		if (str[0] == '`') {
			string name;
			type_var(str, name, env);
			_s.insert(name);
		} else {
			str = str.substr(1);
		}
		return extract_var_name(str, env, _s);
	}
}


// Returen the primitive types in a list, which is represented by vector<id>
int countPrimitiveInAList(const vector<id> arg_list, const Environment &env) {
	int count = 0;
	for (auto vi : arg_list) {
		count += env.get_type(vi).m_num_primitive;
	}
	return count;
}
// string -> vector
vector<id> argList2Vector(string &str, vector<id> &vec_buff, Environment &env, int first_call = 1) {
	string ori = str;
	eatSpace(str);

	if (first_call && str.empty()) {
		vec_buff.push_back(env.get_type_ID(""));
		return vec_buff;
	} else {
		string type_buff;
		if (type(str, type_buff, env)) {
			vec_buff.push_back(env.get_type_ID(type_buff));
			if (eatChar(str, ',')) {
				return argList2Vector(str, vec_buff, env);
			} else {
				return vec_buff;
			}
		} else {
			return vec_buff;
		}
	}
}

/*************************************************************

Unification
unify	-	2 types
most_general	-

/*************************************************************/

int trace(id _id, string &buff, Environment &env, set<id>& visit = set<id>());

int most_general(id _id, string &buff, Environment &env, set<id>& visit = set<id>()) {
	int most_general(const Type &_type, string &buff, Environment &env, set<id>& visit = set<id>());
	return most_general(env.get_type(_id),buff, env,visit);
}

int most_general(const Type &_type, string &buff, Environment &env, set<id>& visit = set<id> ()) throw (MyException) {
	try {
		int ret = 1;
 		if (_type.type == Type::FUNCTYPE) {
			buff += "(";
			for (auto vi : _type.m_arg) {
				set<id> old_visit = visit;
				most_general(vi, buff, env, visit); buff += ",";
				visit = old_visit;
			}
			buff.pop_back();
			buff += ") -> ";
			most_general(_type.m_ret, buff, env, visit);
		} else if (_type.type == Type::VAR) {
			string buff2;
			set<id> visit2 = visit;
			if (trace(_type.m_type_id, buff2, env, visit)) {
				buff += buff2;
			} else {
				buff = "BOTTOM";
				throw(MyException("BOTTOM"));
			}
			visit = visit2;
		} else if (_type.type == Type::PRIMITIVE) {
			buff += _type.m_str;
		} else if (_type.type == Type::LISTTYPE) {
			buff += "[";
			string inner_type = _type.m_str;
			inner_type = inner_type.substr(1);
			inner_type.pop_back();
			most_general(env.get_type(inner_type), buff, env, visit);
			buff += "]";
		} else {
			//throw(MyException("BOTTOM"));
		}
		return ret;
	} catch (MyException ex) {
		throw(ex);
	}
}


int most_general_with_update(const Type &_type, string &buff, Environment &env, set<id> &visit = set<id>()) {
	int ret = most_general(_type, buff, env, visit);
	if (ret && type(string(buff), string(), env)) {
		env.union_(_type, env.get_type(buff));
	}
	return ret;
}

int trace(id _id, string &buff, Environment &env, set<id> &visit)  throw (MyException) {
	int ret = 1;
	int root = env.root(_id);
	vector<id> &vect = env.m_v_sets[root];
	Type this_type = env.get_type(vect[0]);
	if (visit.count(_id)) {
		throw(MyException("BOTTOM"));
	} else {
		visit.insert(_id);
		env.sort_sets(root);
		if ( (this_type.type == Type::PRIMITIVE || this_type.type == Type::VAR)) {
			buff += env.get_type(vect[0]).m_str;
		} else {
			string this_most_general_from__id;
			most_general(vect[0], this_most_general_from__id, env,visit);
			buff += this_most_general_from__id;
			// update in the vect
			
			string type_for_registering = this_most_general_from__id;
			if (type(type_for_registering, string(), env)) {
				env.union_(root, env.get_type_ID(this_most_general_from__id));
			}
			
		}
	}
	return ret;
}

int unify(const Type &type1, const Type &type2, Environment &env) throw (MyException){
	int unify(id _id1, id _id2, Environment &env);
	int unify(const Type &type1, const Type &type2, Environment &env);
	int ret = 1;
	/*	Unify the most general of type1, type2	*/
	// get most_general type
	string str_most_general_type1, str_most_general_type2;
	most_general(type1, str_most_general_type1, env);
	most_general(type2, str_most_general_type2, env);
	// register these 2 most general types
	type(string(str_most_general_type1), string(), env);
	type(string(str_most_general_type2), string(), env);
	// Get type
	Type &mgt1 = env.get_type(str_most_general_type1);
	Type &mgt2 = env.get_type(str_most_general_type2);
	
	if ( mgt1.m_type_id == mgt2.m_type_id) {
		ret = 1;
	}
	// if () unify !()  OR !() unify ()
	else if (((mgt1.type == Type::NIL) ^ (mgt2.type == Type::NIL))) {
		throw (MyException("BOTTOM"));
	}
	// At least one of type12 is var
	else if (mgt1.type == Type::VAR || mgt2.type == Type::VAR) {
		// make sure they dont have the same 
		if (mgt1.type == Type::VAR) {
			if (extract_var_name(string(mgt2.m_str), env).count(mgt1.m_str)) {
				throw (MyException("BOTTOM"));
			}
		} else {
			if (extract_var_name(string(mgt1.m_str), env).count(mgt2.m_str)) {
				throw (MyException("BOTTOM"));
			}
		}
		ret = env.union_(type1.m_type_id, mgt2.m_type_id);
	} 
	// Not var and !same type
	else if(mgt1.type != mgt2.type){
		throw(MyException("BOTTOM"));
		ret = 0;
	}
	// same type
	else {
		// 2 functypes
		if (mgt1.type == Type::FUNCTYPE) {
			// Arg not match
			if (mgt1.m_arg.size() != mgt2.m_arg.size()) {
				throw(MyException("BOTTOM"));
				ret = 0;
			} else {
				// Unify arglist
				for (int i = 0; ret && i < mgt1.m_arg.size(); i++) {
					ret &= unify(mgt1.m_arg[i], mgt2.m_arg[i], env);
				}
				// Arg AC, unify ret
				if (ret) {
					ret = unify(mgt1.m_ret, mgt2.m_ret, env);
				} else {
					throw(MyException("BOTTOM"));
				}
			}
		}
		// LISTTYPE || PRIMITIVE
		else {
			// LISTTYPE
			if (mgt1.type == Type::LISTTYPE) {
				// unify the inner part
				string str_type1 = mgt1.m_str;
				string str_type2 = mgt2.m_str;
				str_type1 = str_type1.substr(1);	str_type1.pop_back();
				str_type2 = str_type2.substr(1);	str_type2.pop_back();
				ret = unify(env.get_type(str_type1), env.get_type(str_type2), env);
				// unify the whole part
				env.union_(mgt1, mgt2);
			} else if(mgt1.type == Type::PRIMITIVE){
				// Primitive
				if (mgt1.m_type_id != mgt2.m_type_id) {
					throw(MyException("BOTTOM"));
					ret = 0;
				} else {
					// Same primitive type, do nothing
				}
			} else {
				// 2(), Do nothing
			}
		}
	}
	return ret;
}

int unify(id _id1, id _id2, Environment &env) {
	return unify(env.get_type(_id1), env.get_type(_id2), env);	
}


/***************************************************

Parser

***************************************************/


/**********************************************
<VARNAME> :: = [a - zA - Z][a - zA - Z0 - 9] *
**********************************************/
int var_name(char *name) {
	return valid_name(name);
}

/**********************************************
<TYPEVAR> :: = ¡®`¡¯ <VARNAME>
**********************************************/
int type_var(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	
	if (str[0] == '`'){
		str = str.substr(1);
		int name_length = valid_name(str);
		buff = str.substr(0, name_length);
		buff = "`" + buff;
		str = str.substr(name_length);
		// register type
		if (!env.is_type_exists(buff)) {
			id _id = env.id_generator();
			Type newType(_id, buff,Type::VAR, 0,1, _id);
			env.add_type(newType);
		}
		return Type::VAR;
	} else {
		str = ori;
		return 0;
	}
}

/**********************************************
<PRIMITIVE_TYPE> ::= ¡®int¡¯ | ¡®real¡¯ | ¡®str¡¯
**********************************************/
int primitive_type(string &str, string& buff, Environment &env) {
	string ori = str;
	eatSpace(str);

	int ret = Type::PRIMITIVE;
	if ( isPrefix(str.c_str(),"int")) {
		buff.assign("int");
	} else if(isPrefix(str.c_str(), "real")){
		buff.assign("real"); 
	} else if (isPrefix(str.c_str(), "str")) {
		buff.assign("str");
	} else {
		str = ori;
		ret = 0;
	}
	if (ret) {
		str = str.substr(buff.length());
		// register type
		if (!env.is_type_exists(buff)) {
			id _id = env.id_generator();
			Type newType(_id,buff,Type::PRIMITIVE,1,0,_id);
			env.add_type(newType);
		}
	}
	return ret;
}

/**********************************************
<ARGLIST> ::= <TYPE> ¡®,¡¯ <ARGLIST> | <TYPE>
Note:
	<ARGLIST> ::= <TYPE> ( epslion | ',' <ARGLIST> ) , this will avoid parsing twice
**********************************************/
int arg_list(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);

	int ret = 1;
	string type_buff;
	if (type(str, type_buff, env)) {
		string ori2 = str;
		if (eatChar(str, ',') && arg_list(str, buff, env)) {
			buff = type_buff + "," + buff;
		} else {
			str = ori2;
			buff += type_buff;
		}
	} else {
		str = ori;
		ret = 0;
	}
	return ret;
}

/*******************************************************
<FUNCTYPE> ::= ¡®(¡® <ARGLIST> ¡®)¡¯ -> <TYPE> | ¡®(¡® ¡®)¡¯ -> <TYPE>
*******************************************************/
int func_type(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	
	int ret = Type::FUNCTYPE;
	buff.swap(string());
	string arg_list_buff;
	string type_buff;
	if (eatChar(str, '(') && arg_list(str, arg_list_buff, env) && eatChar(str, ')') && eatPrefix(str,"->") && type(str, type_buff,env)) {
		buff += "(" + arg_list_buff + ") -> " + type_buff;
	} else {
		// try another parse tree
		str = ori;
		if (eatChar(str, '(') && eatChar(str, ')') && eatPrefix(str,"->") && type(str, type_buff,env)) {
			buff += "() -> " + type_buff;
			// add "" as a special type
			if (!env.is_type_exists("")) {
				Type newType(env.id_generator(),"",Type::NIL);
				env.add_type(newType);
			}
		} else {			
			str = ori;
			ret = 0;
		}
	}
	// This is a functype, add it
	if (ret) {
		if (!env.is_type_exists(buff)) {
			Type newType(env.id_generator());
			id type_ret = env.get_type_ID(type_buff);

			newType.m_ret = type_ret;
			string arg_list_buff2 = arg_list_buff;
			newType.m_arg = argList2Vector(arg_list_buff2,vector<id>(),env);
			newType.m_str = buff;
			newType.m_num_primitive = countPrimitiveInAList(newType.m_arg, env) + env.get_type(type_buff).m_num_primitive;
			newType.m_num_vars = newType.m_arg.size()-newType.m_num_primitive + env.get_type(type_buff).m_num_vars;
			newType.type = Type::FUNCTYPE;
			env.add_type(newType);
		} else {
			// Do nothing
		}
	} else {
		// This is not a type, Do nothing to ret
	}
	return ret;
}

/**********************************************
<LISTTYPE> ::= ¡®[¡® <TYPE> ¡®]¡¯
**********************************************/
int list_type(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	int ret = Type::LISTTYPE;
	if (eatChar(str, '[') && type(str, buff, env) && eatChar(str, ']')) {
		string buff2 = buff; // without "[]"
		buff = "[" + buff + "]";
		if (!env.is_type_exists(buff)) {
			id _id = env.id_generator();
			int _num1 = env.get_type(buff2).m_num_primitive;
			int _num2 = env.get_type(buff2).m_num_vars;
			Type newType(_id, buff,Type::LISTTYPE, _num1,_num2, _id);
			env.add_type(newType);
		}
	} else {
		str = ori;
		ret = 0;
	}
	return ret;
}

/**********************************************
<TYPE> ::= <TYPEVAR> | <PRIMITIVE_TYPE> | <FUNCTYPE> | <LISTTYPE>;
**********************************************/
int type(string &str, string &buff, Environment &env) {
	string ori = str;
	eatSpace(str);
	int ret = func_type(str, buff, env) || type_var(str, buff,env) || primitive_type(str, buff,env) || list_type(str, buff, env);
	if (!ret) {
		str = ori;
	}
	return ret;	
}


/**********************************************
Name:	unification_query
Arg:	str, buff
Func:	Execute an unification query
**********************************************/
int unification_query(string &str, string &type1,string &type2, Environment &env) {
	int ret = 1;
	ret = type(str, type1, env) && eatChar(str, '&') && type(str, type2, env);// && eatChar(str, '\n');
	if (ret) {
		try {
			ret = unify(env.get_type(type1), env.get_type(type2), env);
			string buff;
			most_general(env.get_type(type1), buff, env,set<id>());
			if (!ret) {
				printf("BOTTOM\n");
			} else {
				cout << buff << endl;
			}
		} catch (MyException ex) {
			cout << ex.what() << endl;
			ret = 0;
		}
	} else {
		printf("ERR\n");
	}
	return ret;
}

/**********************************************
Name:	query_list
Arg:	str, buff
Return:	1 if success, 0 otherwise
Func:	Execute a query list
**********************************************/
int query_list(string &str, string &type1, string &type2, Environment &env) {
	return unification_query(str, string(), string(), env) && query_list(str, string(), string(), env);
}

int main() {
	Environment env;
	bool loop = true;
	while (loop) {
		string input;
		string buff, buff2;
		getline(cin, input);
		eatSpace(input);
		if (isPrefix(input.c_str(),"QUIT")) {
			loop = false;
		}
		else {
			loop = unification_query(input, buff, buff2, env);
		}
	}
	return 0;
}

/*
Test Sample:
(int) -> int & `a
(int) -> `b & `a

str & `c
(int) -> `c & `BUFF1
(int) -> `b & `BUFF2
(int) -> `a & `BUFF3
(real) -> `BUFF3 & `BUFF4


(int) -> int & `a
(int) -> `b & `a

(([int],[str],str,`B) -> (int,int,real) -> `A) -> `RST & (`D) -> int
`D & `BUFF1
`RST & `BUFF2
QUIT

`a & [`b]
`a & [str]
QUIT


[([[[()->`c]]])->`D] & `b
`b & [(`a) -> int]
`D & `BUFF1
`a & `BUFF2
QUIT

()->int & `a
`a & (`b) -> `c
QUIT
BOTTOM

(`a)->(`a)->int & `b
(`b)->(`b)->(`a) ->real & `c
QUIT


(int) -> `a & `z
(int) -> `z & `a
BOTTOM

(str) -> `a  & `b
`a & (str) -> `c
(str)->`b & `c
BOTTOM

(str) -> `a  & `b
`a & (str) -> `c
(`b)->int & `c
BOTTOM

[`a] & [[[[`D]]]]
`a & [[[str]]]
`D & `BUFF
QUIT

[`a] & [[[()->`D]]]
`a & `BUFF
`BUFF & [[()->int]]
`D & `BUFF2
QUIT



`aa & `bb
`aa & `cc
`bb & `dd
`dd & ()->[int]
`aa & `bb
`cc & `dd
[int] & [`ff]

`aD & `bb
`aD & (`cc, `dd, `ff) -> [`ee]
`bb & (int,str,real) -> [(     )-> `gg]


() -> [ (int,str, `laaSS22) -> [`FD3]] & () -> [(`sd, `wq, real)->[str]]

*/