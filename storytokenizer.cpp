#include "storytokenizer.hpp"
#include <cstring>
#include <string>
#include <iostream>
#include <utility>


using namespace std;

// STORY TOKENIZER CLASS FUNCTIONS  
//---------------------------------------------------------------------
StoryTokenizer::StoryTokenizer(string story)
{
    theStory = story;
    foundBegin = 0;
    foundEnd = 0;
    foundNameBegin = 0;
    foundNameEnd = 0;

}

PassageToken StoryTokenizer::nextPassage()
{
    string passage, name;
    foundBegin = theStory.find("<tw-passagedata ", foundEnd);

    // </tw-passagedata is 17 characters long so we add 17 to include end tag
    foundEnd = theStory.find("</tw-passagedata>", foundBegin) + 17;
    passage = theStory.substr(foundBegin, foundEnd-foundBegin);

    
    foundNameBegin = passage.find("name=", foundNameEnd)+6;
    for (int i = foundNameBegin; i < passage.length(); i++)
    {
        if (passage.at(i) == '"')
        {
            foundNameEnd = passage.find('"', foundNameBegin);
            break;
        }
    }
    name = passage.substr(foundNameBegin, foundNameEnd-foundNameBegin);
    foundNameBegin = 0;
    foundNameEnd = 0;    


    return PassageToken(passage, name);
}


bool StoryTokenizer::hasNextPassage()
{
    // If .find fails it returns string::npos
   if (theStory.find("<tw-passagedata ", foundEnd) != string::npos)
        return true;
   else
        return false;
}
//---------------------------------------------------------------------


// PASSAGE TOKENIZER CLASS FUNCTIONS
//---------------------------------------------------------------------
PassageTokenizer::PassageTokenizer(string passage)
{
    // constructs text to be tokenized and finds beginning after opening tag
    passageLine = passage;
    cmdLocation = passageLine.find(">") + 1;
    varLocation = 0;

}


SectionToken PassageTokenizer::nextSection()        
{
    type_t sectionType;
    string innerSection;

    // substrings fragments to find StoryGuides, blocks, text, and links
    if (passageLine.substr(cmdLocation, 2) == "[[")
    {
        string nameOfPassage, redirectName;
        int nameStart = 0;
        int varStart = 0;
        sectionStart = passageLine.find("[[", cmdLocation)+2;
        cmdLocation = passageLine.find("]]", sectionStart);

        innerSection = passageLine.substr(sectionStart, cmdLocation-sectionStart);
        
        sectionType = LINK;

        if (innerSection.find("-&gt;", 0) != string::npos)
        {
            int varName = innerSection.find("-&", 0);   
            redirectName = innerSection.substr(varStart, varName-varStart);

            nameStart = innerSection.find("-&gt;", 0) + 5;
            nameOfPassage = innerSection.substr(nameStart, cmdLocation-nameStart);
            
            cmdLocation += 2;
            return SectionToken(passageLine.substr(sectionStart, (cmdLocation-2)-sectionStart), sectionType, redirectName, nameOfPassage);
        }
        else
        {
             cmdLocation += 2;
             return SectionToken(passageLine.substr(sectionStart, (cmdLocation-2)-sectionStart), sectionType, innerSection, innerSection);
        }
    }

    else if (passageLine.substr(cmdLocation, 5) == "(set:")
    {
        
        string varName, varStatus;
        int dollar;
        sectionStart = passageLine.find("(set", cmdLocation);
        cmdLocation = passageLine.find(")", sectionStart) + 1;
        innerSection = passageLine.substr(sectionStart, cmdLocation-sectionStart);
        
        dollar = innerSection.find("$", 0)+1;
        sectionSpace = innerSection.find(" ", dollar);
        varName = innerSection.substr(dollar, sectionSpace-dollar);

        sectionSpace = innerSection.find(" ", sectionSpace+1)+1;
        varStatus = innerSection.substr(sectionSpace, cmdLocation-sectionSpace);
        varStatus = varStatus.substr(0, varStatus.length()-1);

        sectionType = SET;
        return SectionToken(innerSection, sectionType, varName, varStatus);
    }

    else if (passageLine.substr(cmdLocation, 4) == "(if:")
    {
        int dollar;
        sectionStart = passageLine.find("(if:", cmdLocation);
        cmdLocation  = passageLine.find(")", sectionStart) + 1;

        innerSection = passageLine.substr(sectionStart, cmdLocation-sectionStart);
        dollar = innerSection.find("$", 0)+1;
        sectionSpace = innerSection.find(" ", dollar)+1;
        varName = innerSection.substr(dollar, sectionSpace-dollar);
        sectionSpace = innerSection.find(" ", sectionSpace+1)+1;
        varStatus = innerSection.substr(sectionSpace, cmdLocation-sectionSpace-1);
        varStatus = varStatus.substr(0, varStatus.length()-1);

        sectionType = IF;
        return SectionToken(innerSection, sectionType, varName, varStatus);
    }

    else if (passageLine.substr(cmdLocation, 9) == "(else-if:")
    {
        int dollar;
        sectionStart = passageLine.find("(else-if", cmdLocation)+10;
        cmdLocation = passageLine.find(")", sectionStart)+1;
        
        innerSection = passageLine.substr(sectionStart, cmdLocation-sectionStart);
        dollar = innerSection.find("$", 0)+1;
        sectionSpace = innerSection.find(" ", dollar)+1;
        varName = innerSection.substr(dollar, sectionSpace-dollar);
        sectionSpace = innerSection.find(" ", sectionSpace+1)+1;
        varStatus = innerSection.substr(sectionSpace, cmdLocation-sectionSpace-1);
        varStatus = varStatus.substr(0, varStatus.length()-1);

        sectionType = ELSEIF;
        return SectionToken(innerSection, sectionType, varName, varStatus);
    }

    else if (passageLine.substr(cmdLocation, 6) == "(else:")
    {
        sectionStart = passageLine.find("(else", cmdLocation);
        cmdLocation = passageLine.find(")", sectionStart) + 1;

        sectionType = ELSE;

        
    }

    else if (passageLine.substr(cmdLocation, 7) == "(go-to:")
    {
        int goToLocation = 0, quotLocation = 0;
        sectionStart = passageLine.find("(go-to", cmdLocation);
        cmdLocation = passageLine.find(")", sectionStart) + 1;
        innerSection = passageLine.substr(sectionStart, cmdLocation-sectionStart);
        
        goToLocation = innerSection.find("&quot;", goToLocation)+6;
        quotLocation = innerSection.find("&quot;", goToLocation+1);
        varName = innerSection.substr(goToLocation, quotLocation-goToLocation);

        sectionType = GOTO;
        return SectionToken(innerSection, sectionType, varName, varName);
    
    }
    
    else if (((passageLine.substr(cmdLocation, 1) != "[") && passageLine.substr(cmdLocation, 1) != "("))
    {

        // If the beginning character is not ( or [, the token can't be a link, or block
        // token must be text

        sectionType = TEXT;
        sectionStart = cmdLocation;
        int skipLocation = cmdLocation;
        int temp = cmdLocation;
        if(passageLine.find("(?", skipLocation) == string::npos && passageLine.find("(a", skipLocation) == string::npos && passageLine.find("(Y", skipLocation) == string::npos && passageLine.find("(o", skipLocation) == string::npos)
        {
        if ((passageLine.find("(", cmdLocation) == string::npos) && (passageLine.find("[", cmdLocation) == string::npos))
            cmdLocation = passageLine.find("<", cmdLocation);
        else if (passageLine.find("(", cmdLocation) > passageLine.find("[", cmdLocation))
            cmdLocation = passageLine.find("[", cmdLocation);
        else    
            cmdLocation = passageLine.find("(", cmdLocation);
        }
        else
        {
            if (passageLine.find("(s", skipLocation) < passageLine.find("(?", skipLocation))
                cmdLocation = passageLine.find("(s", skipLocation);
            else if (passageLine.find("(?", skipLocation) != string::npos)
                cmdLocation = passageLine.find(")", skipLocation) + 1;
            else if (passageLine.find("(Y", skipLocation) != string::npos)
                cmdLocation = passageLine.find(")", skipLocation) + 1;
            else if (passageLine.find("(o", skipLocation) != string::npos)
                cmdLocation = passageLine.find(")", skipLocation) + 1;
            else if (passageLine.find("(a", skipLocation) != string::npos)
                cmdLocation = passageLine.find(")", skipLocation) + 1;
            else if ((passageLine.find("(", skipLocation) == string::npos) && (passageLine.find("[", skipLocation) == string::npos))
                cmdLocation = passageLine.find("<", skipLocation);
            else if (passageLine.find("(", skipLocation) > passageLine.find("[", skipLocation))
                cmdLocation = passageLine.find("[", skipLocation);
            else
                cmdLocation = passageLine.find("(", skipLocation);
        }

        sectionType = TEXT;
        return SectionToken(passageLine.substr(sectionStart, cmdLocation-sectionStart), sectionType, "hi", "hi");
    }

    else
         {
           
            sectionStart = passageLine.find("[", cmdLocation);
            cmdLocation = sectionStart + 1;
            int bracketCounter = 1;
            
            cmdLocation++;
            while (bracketCounter != 0)
            {
                if (passageLine.at(cmdLocation) == '[')
                {
                    bracketCounter++;
                }
                if (passageLine.at(cmdLocation) == ']')
                {
                    bracketCounter--;
                }
                
                cmdLocation++;
            }
            sectionType = BLOCK;
            return SectionToken(passageLine.substr(sectionStart+1, cmdLocation - (sectionStart+2)), BLOCK, "hi", "hi");
         }

         return SectionToken(passageLine.substr(sectionStart, cmdLocation - sectionStart), sectionType, "hi", "hi");
    
}

bool PassageTokenizer::hasNextSection()
{   

    // Looks for any StoryGuides present and if it finds a StoryGuide, there must be a next section.
    if (passageLine.find("(set:", cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("(display:", cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("go-to:", cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("[[",  cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("if:", cmdLocation) != string::npos)
        return true;
    else if(passageLine.find("[", cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("else-if:", cmdLocation) != string::npos)
        return true;
    else if (passageLine.find("else:", cmdLocation) != string::npos)
        return true;
    else if (passageLine.substr(cmdLocation, 1) != "<")
        return true;
    else
        return false;


}
//---------------------------------------------------------------------


// PASSAGE TOKEN CLASS FUNCTIONS
//----------------------------------------------------------------------
PassageToken::PassageToken(string passage, string name)
{
   passageText = passage;
   passageName = name;
}

//-----------------------------------------------------------------------


// SECTION TOKEN CLASS FUNCTIONS
//----------------------------------------------------------------------
SectionToken::SectionToken(string secText, type_t secType, string varName, string varStatus)
{
    // constructs SectionToken using the text and type of each fragment.
    sectionText = secText;
    sectionType = secType;
    variableName = varName;
    variableStatus = varStatus;
}


string SectionToken::getText() const
{
    return sectionText;
}

type_t SectionToken::getType() const
{
    return sectionType;
}

string SectionToken::getVarName() const
{
    return variableName;
}

string SectionToken::getVarStatus() const
{
    return variableStatus;
}
//--------------------------------------------------------------------


// StoryGuide CLASS FUNCTIONS
//----------------------------------------------------------------------
StoryGuide::StoryGuide() 
{
    unordered_map<string, bool> storySettings;
    unordered_map<string, int> storyMap;
    vector<PassageToken*> passageCollection;
}

void StoryGuide::setPassageMap(string name, int passNum)
{
    storyMap.insert(make_pair(name, passNum));
}

int StoryGuide::findPassageIndex(string passage) const
{
    return storyMap.find(passage)->second;
}


void StoryGuide::setSetting(string name, string value)
{
    
    unordered_map<string,bool>::iterator i = storySettings.find(name);
    if (i == storySettings.end() || storySettings.size() == 0)
    {
        if (value.compare("true") == 0)
            storySettings.insert(make_pair(name, true));
        else
            storySettings.insert(make_pair(name, false));
    }
    else
        if (value == "true")
            i->second = true;
        else
            i->second = false;
}

string StoryGuide::checkSetting(string key) const
{
    if (key.at(key.length()-1) == ' ')
        key = key.substr(0, key.length()-1);

    if (storySettings.find(key)->second == true)
        return "true";
    else
        return "false";
        
}
//----------------------------------------------------------------------
