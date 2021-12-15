#pragma once

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: conparser.h: Console expression parser interface
*/

//======================================================================
#include "includes.h"
//======================================================================

//======================================================================
#define CM_CVAR_MAX_STRING_LEN 1024
#define CM_INVALID_LIST_ID std::string::npos

#if defined(_WIN32) || defined(_WIN64)
	#define CM_DIR_CHAR '\\'
#elif defined(__linux__)
	#define CM_DIR_CHAR '/'
#endif
//======================================================================

//======================================================================
namespace ConfigMgr {
	class CCVar {
	public: 
		enum cvar_type_e {
			CVAR_TYPE_BOOL,
			CVAR_TYPE_INT,
			CVAR_TYPE_FLOAT,
			CVAR_TYPE_DOUBLE,
			CVAR_TYPE_STRING
		};

		struct cvar_s {
			std::string szName;
			cvar_type_e eType;
			union {
				bool bValue;
				int iValue;
				float fValue;
				double dblValue;
				char szValue[CM_CVAR_MAX_STRING_LEN];
			};

			virtual std::string GetName(void) { return this->szName; }
			virtual cvar_type_e GetType(void) { return this->eType; }
			virtual bool GetBool(void) { return this->bValue; }
			virtual int GetInt(void) { return this->iValue; }
			virtual float GetFloat(void) { return this->fValue; }
			virtual double GetDouble(void) { return this->dblValue; }
			virtual std::string GetString(void) { return this->szValue; }

			virtual void SetBool(bool bValue) { this->bValue = bValue; }
			virtual void SetInt(int iValue) { this->iValue = iValue; }
			virtual void SetFloat(float fValue) { this->fValue = fValue; }
			virtual void SetDouble(double dblValue) { this->dblValue = dblValue; }
			virtual void SetString(std::string szValue) { strcpy_s(this->szValue, szValue.c_str()); }
		};
	private:
		std::vector<cvar_s*> m_vCVars;

		size_t GetCVarId(const cvar_s* pCvar)
		{
			//Get CVar ID by pointer

			for (size_t i = 0; i < this->m_vCVars.size(); i++) { //Iterate through list
				//Compare pointers and return ID if equal
				if (this->m_vCVars[i] == pCvar)
					return i;
			}

			return CM_INVALID_LIST_ID;
		}

		size_t GetCVarId(const std::string& szName)
		{
			//Get CVar ID by pointer

			for (size_t i = 0; i < this->m_vCVars.size(); i++) { //Iterate through list
				//Compare names and return ID if equal
				if (this->m_vCVars[i]->szName == szName)
					return i;
			}

			return CM_INVALID_LIST_ID;
		}

		void Release(void)
		{
			//Release all cvars

			//Free memory
			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				delete this->m_vCVars[i];
			}

			//Clear list
			this->m_vCVars.clear();
		}
	public:
		CCVar() {}
		~CCVar() { this->Release(); }

		cvar_s* Add(const std::string& szName, const cvar_type_e eType, const std::string& szInitialValue)
		{
			//Add CVar

			if (!szName.length())
				return nullptr;

			//Check if already exists
			if (this->Find(szName))
				return nullptr;

			//Allocate memory
			cvar_s* pCVar = new cvar_s;
			if (!pCVar)
				return nullptr;

			//Save data
			pCVar->szName = szName;
			pCVar->eType = eType;

			//Set initial value
			if (szInitialValue.length()) {
				switch (eType) {
				case CVAR_TYPE_BOOL:
					pCVar->bValue = atoi(szInitialValue.c_str()) != 0;
					break;
				case CVAR_TYPE_INT:
					pCVar->iValue = atoi(szInitialValue.c_str());
					break;
				case CVAR_TYPE_FLOAT:
					pCVar->fValue = (float)atof(szInitialValue.c_str());
					break;
				case CVAR_TYPE_DOUBLE:
					pCVar->dblValue = atof(szInitialValue.c_str());
					break;
				case CVAR_TYPE_STRING:
					strcpy_s(pCVar->szValue, szInitialValue.c_str());
					break;
				default:
					break;
				}
			}

			//Add to list
			this->m_vCVars.push_back(pCVar);

			//Return pointer to data
			return pCVar;
		}

		cvar_s* Find(const std::string& szName)
		{
			//Find CVar in list

			if (!szName.length())
				return nullptr;

			//Search in list
			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if ((this->m_vCVars[i]) && (this->m_vCVars[i]->szName == szName)) {
					return this->m_vCVars[i];
				}
			}

			return nullptr;
		}

		bool Set(const std::string& szName, const bool bValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->bValue = bValue;

			return true;
		}

		bool Set(const std::string& szName, const int iValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->iValue = iValue;

			return true;
		}

		bool Set(const std::string& szName, const float fValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->fValue = fValue;

			return true;
		}

		bool Set(const std::string& szName, const double dblValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->dblValue = dblValue;

			return true;
		}

		bool Set(const std::string& szName, const std::string& szValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			strcpy_s(pCVar->szValue, szValue.c_str());

			return true;
		}

		bool SetCast(const std::string& szName, const std::string& szValue)
		{
			//Set casted value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			switch (pCVar->eType) {
				case CVAR_TYPE_BOOL:
					pCVar->bValue = atoi(szValue.c_str()) != 0;
					break;
				case CVAR_TYPE_INT:
					pCVar->iValue = atoi(szValue.c_str());
					break;
				case CVAR_TYPE_FLOAT:
					pCVar->fValue = (float)atof(szValue.c_str());
					break;
				case CVAR_TYPE_DOUBLE:
					pCVar->dblValue = atof(szValue.c_str());
					break;
				case CVAR_TYPE_STRING:
					strcpy_s(pCVar->szValue, szValue.c_str());
					break;
				default:
					return false;
					break;
			}

			return true;
		}

		bool Delete(const size_t uiListId)
		{
			//Delete CVar by cvar list ID

			if (uiListId == CM_INVALID_LIST_ID)
				return false;

			//Free memory
			delete this->m_vCVars[uiListId];

			//Erase from list
			this->m_vCVars.erase(this->m_vCVars.begin() + uiListId);

			return true;
		}

		bool Delete(const cvar_s* pCVar)
		{
			//Delete CVar by cvar pointer

			//Get CVar list ID
			size_t uiListId = this->GetCVarId(pCVar);
			if (uiListId == CM_INVALID_LIST_ID)
				return false;

			//Delete item
			return this->Delete(uiListId);
		}

		bool Delete(const std::string& szName)
		{
			//Delete CVar by cvar name

			//Get CVar list ID
			size_t uiListId = this->GetCVarId(szName);
			if (uiListId == CM_INVALID_LIST_ID)
				return false;

			//Delete item
			return this->Delete(uiListId);
		}
	};

	class CCommand {
	public:
		typedef void (*TCommandCallback)(void);
	public:
		struct command_s {
			std::string szName;
			std::string szDescription;
			TCommandCallback pCallbackRoutine;
		};
	private:
		std::vector<command_s> m_vCmds;

	protected:
		bool Find(const std::string& szName, size_t* pEntryId)
		{
			//Check if a command exists

			for (size_t i = 0; i < this->m_vCmds.size(); i++) {
				if (this->m_vCmds[i].szName == szName) {
					if (pEntryId)
						*pEntryId = i;

					return true;
				}
			}

			return false;
		}
	public:
		CCommand() {}
		~CCommand() { this->m_vCmds.clear(); }

		bool Add(const std::string& szName, const std::string& szDescription, const TCommandCallback pRoutine)
		{
			//Add new command

			if ((!szName.length()) || (!pRoutine))
				return false;

			//Check if already exists
			if (this->Find(szName, nullptr))
				return false;

			//Setup data
			command_s sData;
			sData.szName = szName;
			sData.szDescription = szDescription;
			sData.pCallbackRoutine = pRoutine;

			//Add to list
			this->m_vCmds.push_back(sData);
			
			return true;
		}

		bool Delete(const std::string& szName)
		{
			//Remove command

			if (!szName.length())
				return false;

			size_t uiListId;

			//Get command list ID
			if (!this->Find(szName, &uiListId))
				return false;

			//Erase from list
			this->m_vCmds.erase(this->m_vCmds.begin() + uiListId);

			return true;
		}

		bool Handle(const std::string& szName)
		{
			//Check if command exists and if so call event function

			size_t uiCmdId;

			//Attempt to find entry and retrieve id
			if (!this->Find(szName, &uiCmdId))
				return false;

			//Check pointer
			if (!this->m_vCmds[uiCmdId].pCallbackRoutine)
				return false;

			//Call event function
			this->m_vCmds[uiCmdId].pCallbackRoutine();

			return true;
		}

		const size_t GetCommandCount(void) const
		{
			//Get amount of commands

			return this->m_vCmds.size();
		}

		command_s* GetCommandById(const size_t uiCmdId)
		{
			//Get command data by ID

			if (uiCmdId < this->m_vCmds.size()) {
				return &this->m_vCmds[uiCmdId];
			}

			return NULL;
		}
	};

	class CCmdLine : public CCVar {
	private:
		static const char C_CVAR_CHAR = '+';
		static const char C_ARG_CHAR = '-';
	public:
		struct cmd_arg_s {
			std::string szName;
			std::vector<std::string> vArgs;
		};
	private:
		std::vector<cmd_arg_s*> m_vArgs;
		char** m_ppArgs;
		int m_iCount;

		int HandleArg(const std::string& szName, int iArgStart)
		{
			//Handle current argument

			int iIgnoreCount = 0; //How much arg strings have to be ignored

			//Allocate memory
			cmd_arg_s* pArg = new cmd_arg_s;
			if (!pArg)
				return -1;

			pArg->szName = szName;
			
			for (int i = iArgStart; i < this->m_iCount; i++) { //Loop through args
				//Check if current arg string is a new item and if so break out
				if ((this->m_ppArgs[i][0] == C_CVAR_CHAR) || (this->m_ppArgs[i][0] == C_ARG_CHAR)) {
					break;
				}
				
				//Add item arg to list
				pArg->vArgs.push_back(std::string(this->m_ppArgs[i]));

				//Increment counter so that this item arg is ignored
				iIgnoreCount++;
			}

			//Add to list
			this->m_vArgs.push_back(pArg);

			return iIgnoreCount;
		}

		void Release(void)
		{
			//Release resources
			
			//Free memory
			for (size_t i = 0; i < this->m_vArgs.size(); i++) {
				delete this->m_vArgs[i];
			}

			//Clear list
			this->m_vArgs.clear();
		}
	public:
		CCmdLine() : m_ppArgs(nullptr), m_iCount(0) {}
		CCmdLine(char** ppArgs, const int iArgCount) : m_ppArgs(ppArgs), m_iCount(iArgCount) { this->Collect(); }
		~CCmdLine() { this->Release(); }

		void SetCmdArgs(char** ppArgs, const int iArgCount) { this->m_ppArgs = ppArgs; this->m_iCount = iArgCount; }

		bool Collect(void)
		{
			//Collect all items

			if ((!this->m_ppArgs) || (!this->m_iCount))
				return false;

			//Check if arguments have been provided
			if (this->m_iCount == 1)
				return true;

			for (int i = 1; i < this->m_iCount; i++) { //Loop through args
				//There shouldn't be a nullptr-pointer but if so this is a failure
				if (!this->m_ppArgs[i])
					return false;

				if (this->m_ppArgs[i][0] == C_CVAR_CHAR) { //Check for cvar char
					//Check if we would exceed the limit
					if (i + 1 >= this->m_iCount)
						return false;

					//Attempt to get pointer to value string
					const char* pszCVarValue = this->m_ppArgs[i + 1];
					if (!pszCVarValue)
						return false;

					//Set cvar value
					this->SetCast(std::string(&this->m_ppArgs[i][1]), std::string(pszCVarValue));

					//Ignore cvar value iteration
					i++;

					//Continue with next iteration
					continue;
				} else if (this->m_ppArgs[i][0] == C_ARG_CHAR) { //Check for arg char
					int iIgnoreCount = HandleArg(std::string(&this->m_ppArgs[i][1]), i + 1);
					if (iIgnoreCount == -1)
						return false;

					//Increment according to ignore count
					i += iIgnoreCount;
					
					//Continue with next iteration
					continue;
				} else {
					//Unknown item
					return false;
				}
			}
			
			return true;
		}

		const size_t Count(void)
		{
			//Return amount of items

			return this->m_vArgs.size();
		}

		cmd_arg_s* Item(const size_t uiId)
		{
			//Get item by ID

			if (uiId >= this->m_vArgs.size())
				return nullptr;

			return this->m_vArgs[uiId];
		}
	};

	typedef void(*TUnknownExpression)(const std::string& szExpression);

	class CScriptParser : public CCmdLine, public CCommand {
	private:
		struct script_var_s {
			std::string szName;
			std::string szReplacer;
		};

		//Syntax char elements
		static const char C_SPACE_CHAR = ' ';
		static const char C_TAB_CHAR = '\t';
		static const char C_QUOTE_CHAR = '\"';
		static const char C_BRACKET_START_CHAR = '[';
		static const char C_BRACKET_END_CHAR = ']';
		static const char C_COMMENT_CHAR = '#';
		static const char C_ITEMS_DELIM = ';';

		std::vector<std::string> m_vExpressionItems; //Items for current handled expression
		std::vector<script_var_s> m_vScriptVars; //Script variables

		TUnknownExpression m_pfnUnknownExpressionInform; //Function to call when encountered an unknown expression

		void HandleCurrentItem(bool bClear = false)
		{
			if (this->m_vExpressionItems.size()) { //Check if not empty
				//Handle cvar if it is one
				if ((this->m_vExpressionItems.size() > 1) && (CCVar::SetCast(this->m_vExpressionItems[0], this->m_vExpressionItems[1]))) {
					//Clear list if desired
					if (bClear)
						this->m_vExpressionItems.clear();

					return;
				}

				//Handle command if it is one
				if (CCommand::Handle(this->m_vExpressionItems[0])) {
					//Clear list if desired
					if (bClear)
						this->m_vExpressionItems.clear();

					return;
				}

				//Inform about unknown expression
				if (this->m_pfnUnknownExpressionInform)
					this->m_pfnUnknownExpressionInform(this->m_vExpressionItems[0]);

				//Clear list if desired
				if (bClear)
					this->m_vExpressionItems.clear();
			}
		}

		size_t FindScriptVariable(const std::string& szName)
		{
			//Find script variable if exists

			for (size_t i = 0; i < this->m_vScriptVars.size(); i++) {
				if (this->m_vScriptVars[i].szName == szName) {
					return i;
				}
			}

			return std::string::npos;
		}
	public:
		CScriptParser() : m_pfnUnknownExpressionInform(nullptr) {}

		CScriptParser(const TUnknownExpression pfnFunction) : m_pfnUnknownExpressionInform(pfnFunction) {}

		CScriptParser(const TUnknownExpression pfnFunction, const std::string& szExpression) : m_pfnUnknownExpressionInform(pfnFunction)
		{
			this->Parse(szExpression);
		}

		~CScriptParser() { this->m_vExpressionItems.clear(); }

		bool SetUnknownExpressionInformer(const TUnknownExpression pfnFunction)
		{
			//Set event function for unknown expressions

			if (!pfnFunction)
				return false;

			this->m_pfnUnknownExpressionInform = pfnFunction;

			return true;
		}

		void SetScriptVariable(const std::string& szName, const std::string& szValue)
		{
			//Set script variable

			size_t uiScriptVar = this->FindScriptVariable(szName);
			if (uiScriptVar != std::string::npos) {
				this->m_vScriptVars[uiScriptVar].szReplacer = szValue;
			} else {
				script_var_s sVar;
				sVar.szName = szName;
				sVar.szReplacer = szValue;
				this->m_vScriptVars.push_back(sVar);
			}
		}

		std::string ReplaceVariables(const std::string& szExpression)
		{
			//Replace variable values

			std::string szResult = szExpression;

			for (size_t i = 0; i < this->m_vScriptVars.size(); i++) {
				size_t uiExprPos = szResult.find("%" + this->m_vScriptVars[i].szName + "%");
				while (uiExprPos != std::string::npos) {
					szResult = szResult.replace(szResult.begin() + uiExprPos, szResult.begin() + uiExprPos + this->m_vScriptVars[i].szName.length() + 2, this->m_vScriptVars[i].szReplacer);
					uiExprPos = szResult.find("%" + this->m_vScriptVars[i].szName + "%");
				}
			}

			return szResult;
		}

		bool Parse(const std::string szExpression)
		{
			//Parse expression

			if (!szExpression.length())
				return false;

			bool bInQuotes = false; //Whether in-quote context is true or false
			bool bGotNonSpaceChar = false; //Whether if a non-space char has been encountered
			short swBracketCounter = 0; //Bracket counter
			std::string szTemp(""); //Temporary string
			
			this->m_vExpressionItems.clear();
			
			for (size_t i = 0; i <= szExpression.length(); i++) { //Loop through string chars
				//If current char is a quote char then toggle indicator value
				if (szExpression[i] == C_QUOTE_CHAR) {
					bInQuotes = !bInQuotes;

					//Ignore only if not in brackets
					if (!swBracketCounter)
						continue;
				}

				//Set indicator to true if case is true
				if (((!bGotNonSpaceChar) && ((szExpression[i] != C_SPACE_CHAR) || (szExpression[i] != C_TAB_CHAR)))) {
					bGotNonSpaceChar = true;
				}

				//Ignore leading spaces
				if (!bGotNonSpaceChar)
					continue;

				if (!bInQuotes) { //Outside of an expression item
					//If current char is a space/tab char and also the next char then ignore current char
					if (((szExpression[i] == C_SPACE_CHAR) || (szExpression[i] == C_TAB_CHAR)) && ((szExpression[i + 1] == C_SPACE_CHAR) || (szExpression[i + 1] == C_TAB_CHAR))) {
						continue;
					}

					//Check for brackets
					if (szExpression[i] == C_BRACKET_START_CHAR) {
						swBracketCounter++; //Increment counter

						//Ignore only if this is the first one
						if (swBracketCounter == 1)
							continue;
					} else if (szExpression[i] == C_BRACKET_END_CHAR) {
						swBracketCounter--; //Decrement counter

						//Ignore only if this is the last one
						if (swBracketCounter == 0)
							continue;
					}

					//Check if a delimiter char has been found or comment or end of string reached
					if  ((!swBracketCounter) && ((szExpression[i] == C_SPACE_CHAR) || (szExpression[i] == C_TAB_CHAR) || (szExpression[i] == C_BRACKET_END_CHAR) || (szExpression[i] == C_COMMENT_CHAR) || (szExpression[i] == C_ITEMS_DELIM) || (!szExpression[i]))) {
						//The first expression may not be empty
						if ((!this->m_vExpressionItems.size()) && ((i > 0) && (!szTemp.length()))) {
							szTemp.clear(); //Clear string
							continue;
						}

						this->m_vExpressionItems.push_back(szTemp); //Add item to list

						//Break out if end reached
						if (szExpression[i] == C_COMMENT_CHAR)
							break;

						szTemp.clear(); //Clear temp string
						//Handle expression now if item delimiter has been found
						if (szExpression[i] == C_ITEMS_DELIM) {
							this->HandleCurrentItem(true); //Handle current item
							bGotNonSpaceChar = false; //Reset indicator
						}

						continue; //Ignore current char
					}
				}

				//Append current char
				szTemp += szExpression[i];
			}

			this->HandleCurrentItem(); //Handle current item

			return true;
		}

		const size_t ExpressionItemCount(void)
		{
			//Return count

			return this->m_vExpressionItems.size();
		}

		std::string ExpressionItemValue(const size_t uiId)
		{
			//Get expression item by id

			std::string szResult("");

			if (uiId < this->m_vExpressionItems.size()) {
				szResult = this->m_vExpressionItems[uiId];
			}

			return szResult;
		}
	};

	class CConfigInt : public CScriptParser {
	private:
		std::string m_szScriptDir; //Full path of script directory
	public:
		CConfigInt() {}
		CConfigInt(const std::string& szScriptDir) : m_szScriptDir(szScriptDir) {}
		CConfigInt(const std::string& szScriptDir, const std::string szScript) : m_szScriptDir(szScriptDir)
		{
			this->Execute(szScript);
		}

		~CConfigInt() {}

		void SetScriptDirectory(const std::string& szScriptDir)
		{
			//Save script directory

			this->m_szScriptDir = szScriptDir;

			if (this->m_szScriptDir[this->m_szScriptDir.length()-1] != CM_DIR_CHAR)
				this->m_szScriptDir += CM_DIR_CHAR;
		}

		bool Execute(const std::string& szScriptFile)
		{
			//Execute script file

			std::ifstream hFile(this->m_szScriptDir + szScriptFile); //Open file for reading
			if (hFile.is_open()) { //If file could be opened
				std::string szCurrentLine("");

				while (!hFile.eof()) { //While not at end of file
					std::getline(hFile, szCurrentLine, '\n'); //Read current line
					
					//Ignore empty lines
					if (!szCurrentLine.length())
						continue;

					//Parse current line
					CScriptParser::Parse(CScriptParser::ReplaceVariables(szCurrentLine));
				}

				hFile.close(); //Close file

				return true;
			}

			return false;
		}
	};
}
//======================================================================
