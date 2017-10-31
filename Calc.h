#ifndef CALC_H_INCLUDED
#define CALC_H_INCLUDED

#include <string>
using std::string;
#include <cstdlib>
using std::atof;
#include <stack>
using std::stack;
#include <stdexcept>
using std::logic_error;

namespace Calc
{
/**< 从表达式中 index 处开始，抽出一个合法数字的字符串形式
 * 并移动 index 至数字后一个字符位置，可能越界 */
string extractNum(const string &Exp, unsigned index);
/**< 计算运算符优先级：'+''-'=1; '*''/''%'=2; else=0 */
int precOp(const char &c);
/**< 规整中缀表达式，加 0 */
void normExp(string &inExp) throw(logic_error);
/**< 计算中缀表达式的值 */
double calcInExp(const string &inExp) throw(logic_error);
/**< 将中缀表达式转为后缀表达式 */
string inToPost(const string &inExp);
/**< 计算后缀表达式的值 */
double calcPostExp(const string &postExp) throw(logic_error);
}

namespace Calc
{
/** \brief 从表达式中 index 处开始，抽出一个合法数字的字符串形式
 * 	并移动 index 至数字后一个字符位置
 *
 * \param Exp 表达式
 * \param index 表达式下标索引
 * \return string 数字的字符串形式
 */
inline string extractNum(const string &Exp, unsigned &index)
{
	bool sciNum = false; // 科学记数法
	bool numPoint = false; // 小数点标记
	char c = Exp.at(index);
	string strNum;
	strNum += c;
	++index;
	while (index < Exp.size())
	{
		c = Exp.at(index);
		if (c >= '0' && c <= '9') strNum += c;
		else if ((c == '.') && (!numPoint))
		{
			strNum += c;
			numPoint = true;
		}
		else if ((!sciNum) && (c == 'e' || c == 'E'))
		{
			strNum += c;
			sciNum = true;
		}
		else break;
		++index;
	}
	return strNum;
}

/** \brief 计算运算符优先级
 *
 *  \param [in] c 运算符
 *  \return 优先级
 *  \details '+''-'=1; '*''/''%'=2; else=0
 */
inline int precOp(const char &c)
{
	int preOp = 0;
	switch (c)
	{
	case '+':
	case '-': preOp = 1; break;
	case '*':
	case '/':
	case '%': preOp = 2; break;
	}
	return preOp;
}

/** \brief 规整中缀表达式，加 0
 *
 *  \param [in] inExp 中缀表达式
 *  \details 直接改变原表达式
 */
inline void normExp(string &inExp) throw(logic_error)
{
	if (inExp.empty()) return;
	char c = inExp.at(0);
	char d = '0';
	// 前置正负号前加 0
	if (c == '+' || c == '-') inExp.insert(0, 1, '0');
	else if (c < '0' || c > '9')
	{
		string er = "\n>>_<< 表达式不对。错误在 \"";
		er += c;
		er += "\" 处。\n";
		throw logic_error(er.c_str());
	}
	// 将 (-3)+1 变成 (0-3)+1
	for (unsigned i = 2; i < inExp.size(); ++i)
	{
		c = inExp.at(i - 1);
		d = inExp.at(i);
		if (c == ' ') { ++i; continue; }// 跳过空格
		else if (c == '(')
			if (d == '+' || d == '-')
			{
				inExp.insert(i - 1, "0");
				++i;
			}
	}
}

/** \brief 计算中缀表达式的值
 *
 *  \param [in] inExp 规整后的中缀表达式
 *  \return 计算结果
 *
 *  \details 表达式有误则会抛出异常
 */
inline double calcInExp(const string &inExp) throw(logic_error)
{
	if (inExp.empty()) throw logic_error("\n空的表达式。\n");
	char c = 0;
	std::stack<double> dStack; // 存储操作数和中间结果
	std::stack<char> cStack; // 存储运算符
	for (unsigned i = 0; i < inExp.size(); ++i)
	{
		c = inExp.at(i);
		if (c == ' ') continue; // 跳过空格
		else if (c >= '0' && c <= '9')
		{
			string strValue = extractNum(inExp, i);
			--i; // 不加不行
			dStack.push(atof(strValue.c_str()));
		}
		else if (c == '(') cStack.push(c);
		else if (c == ')')
		{
			while (cStack.top() != '(')
			{
				double d2 = dStack.top(); dStack.pop();
				double d1 = dStack.top(); dStack.pop();
				char ch = cStack.top(); cStack.pop();
				switch (ch)
				{
				case '+': d1 = d1 + d2; break;
				case '-': d1 = d1 - d2; break;
				case '*': d1 = d1 * d2; break;
				case '/': d1 = d1 / d2; break;
				case '%':
					d1 = static_cast<int>(d1) % static_cast<int>(d2);
					break;
				default: break;
				}
				dStack.push(d1);
			}
			cStack.pop();
		}
		else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
		{
			if (cStack.empty()) cStack.push(c);
			else if (precOp(c) > precOp(cStack.top())) cStack.push(c);
			else
			{
				while ((!cStack.empty()) && precOp(c) <= precOp(cStack.top()))
				{
					double d2 = dStack.top(); dStack.pop();
					double d1 = dStack.top(); dStack.pop();
					char ch = cStack.top(); cStack.pop();
					switch (ch)
					{
					case '+': d1 = d1 + d2; break;
					case '-': d1 = d1 - d2; break;
					case '*': d1 = d1 * d2; break;
					case '/': d1 = d1 / d2; break;
					case '%':
						d1 = static_cast<int>(d1) % static_cast<int>(d2);
						break;
					default: break;
					}
					dStack.push(d1);
				}
				cStack.push(c);  // 好像都要 push(c) -_-||
			}
		}
		else
		{
			string er = "\n>>_<< 表达式不对。错误在 \"";
			er += c; er += "\" 处。\n";
			throw logic_error(er.c_str());
			break;
		}
	}
	while (!cStack.empty()) // 再有错误就在栈上崩掉吧
	{
		double d2 = dStack.top(); dStack.pop();
		double d1 = dStack.top(); dStack.pop();
		char ch = cStack.top(); cStack.pop();
		switch (ch)
		{
		case '+': d1 = d1 + d2; break;
		case '-': d1 = d1 - d2; break;
		case '*': d1 = d1 * d2; break;
		case '/': d1 = d1 / d2; break;
		case '%':
			d1 = static_cast<int>(d1) % static_cast<int>(d2);
			break;
		default: break;
		}
		dStack.push(d1);
	}
	cStack.pop();
	double d = dStack.top();
	return d;
}

/** \brief 将中缀表达式转为后缀表达式
 *
 *  \param [in] inExp 规整后的中缀表达式
 *  \return 后缀表达式
 *  \details Details
 */
inline string inToPost(const string & inExp)
{
	if (inExp.empty()) return string("");
	std::stack<char> postStack;
	string postExp;
	char c;
	for (unsigned i = 0; i < inExp.size(); ++i)
	{
		c = inExp.at(i);
		if (c == ' ') continue; // 跳过空格
		else if (c >= '0' && c <= '9') // 操作数：数字
		{
			postExp += extractNum(inExp, i);
			--i;
			postExp += ' '; // 空格用于划分数字
		}
		else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') // 操作子：运算符
		{
			while ((!postStack.empty()) && (postStack.top() != '(') &&
				(precOp(postStack.top()) >= precOp(c)))
			{
				postExp += postStack.top();
				postExp += ' ';
				postStack.pop();
			}
			postStack.push(c);
		}
		else if (c == '(') postStack.push(c); // 左括号
		else if (c == ')') // 右括号
		{
			while (postStack.top() != '(')
			{
				postExp += postStack.top();
				postExp += ' ';
				postStack.pop();
			}
			postStack.pop();
		}
		else
		{
			string er = "\n>>_<< 表达式不对。错误在 \"";
			er += c;
			er += "\" 处。\n";
			throw logic_error(er.c_str());
		}
	}
	while (!postStack.empty())
	{
		postExp += postStack.top();
		postExp += ' ';
		postStack.pop();
	}
	return postExp;
}

/** \brief 计算后缀表达式的值
 *
 *  \param [in] postExp 后缀表达式
 *  \return 计算结果
 *  \details 表达式有误则会抛出异常
 */
inline double caclPostExp(const string & postExp) throw(logic_error)
{
	if (postExp.empty())
		throw logic_error("\n空的表达式。\n");
	char c = 0;
	std::stack<double> dStack;
	for (unsigned i = 0; i < postExp.size(); ++i)
	{
		c = postExp.at(i);
		if (c == ' ') continue; // 跳过空格
		else if (c >= '0' && c <= '9')
		{
			string strValue = extractNum(postExp, i);
			dStack.push(atof(strValue.c_str()));
		}
		else
		{
			double d2 = dStack.top(); dStack.pop();
			double d1 = dStack.top(); dStack.pop();
			switch (c)
			{
			case '+': d1 = d1 + d2; break;
			case '-': d1 = d1 - d2; break;
			case '*': d1 = d1 * d2; break;
			case '/': d1 = d1 / d2; break;
			case '%':
				d1 = static_cast<int>(d1) % static_cast<int>(d2);
				break;
			default:
				string er = "\n>>_<< 表达式不对。错误在 \"";
				er += c;
				er += "\" 处。\n";
				throw logic_error(er.c_str());
				break;
			}
			dStack.push(d1);
		}
	}
	double d = dStack.top();
	return d;
}
}

#endif // CALC_H_INCLUDED
