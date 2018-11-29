#ifndef __STORYTOKENIZER_HPP
#define __STORYTOKENIZER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

enum type_t {LINK, SET, GOTO, IF, ELSEIF, ELSE, BLOCK, TEXT};


class SectionToken 
{
    public:
        SectionToken(string secText, type_t secType, string variableName, string variableStatus);
        string getText() const;
        type_t getType() const;
        string getVarName() const;
        string getVarStatus() const;
    private:
        string sectionText;
        type_t sectionType;
        string variableName;
        string variableStatus;

};


class PassageTokenizer 
{
    public:
        PassageTokenizer();
        PassageTokenizer(string passage);
        SectionToken nextSection();
        bool hasNextSection();
        bool hasLink();
    private:
        int cmdLocation;
        int sectionStart;
        int varLocation;
        int sectionSpace;
        string passageLine;
        string innerSection;
        string varName;
        string varStatus;
};



class PassageToken 
{
    public:
        PassageToken(string passage, string name);
        string getText() const { return passageText; };
        string getName() const { return passageName; };
    private:
        string passageText;
        string passageName;
};


class StoryTokenizer 
{
    public:
        StoryTokenizer(string story);
        PassageToken nextPassage();
        bool hasNextPassage();
    private:
        string theStory;
        size_t foundBegin;
        size_t foundEnd;
        size_t foundNameBegin;
        size_t foundNameEnd;
};


class StoryGuide
{
    public:
        StoryGuide();
        void setSetting(string variable, string value);
        void setPassageMap(string name, int passNum);
        int findPassageIndex(string passage) const;
        string checkSetting(string varToCheck) const;
        
    protected:
        unordered_map<string, bool> storySettings;
        unordered_map<string, int> storyMap;
        
};


#endif
