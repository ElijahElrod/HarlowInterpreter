#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "commandFunctions.h"



bool hasLinks(vector<string> name)
{
  if (name.size() != 0)
    return true;
  else
    return false;
}


void callGoTo(SectionToken stok, string& redirect, bool& flag)
{
  redirect = stok.getVarName();
  flag = true;
}

void callLink(SectionToken stok, vector<string>& links, vector<string>& redirectLinks)
{
     if (stok.getVarName() == stok.getVarStatus())
       {
          links.push_back(stok.getVarName());
          redirectLinks.push_back(" ");
        }
      else
        {
          links.push_back(stok.getVarName()); 
          redirectLinks.push_back(stok.getVarStatus());
        }
      cout << stok.getText();
}

void callBlock(SectionToken stok, bool flag1, bool& flag2)
{
  if (flag1 == true && flag2 == false)
  {
          cout << stok.getText();
          flag2 = true;
  }
      
}

void callIf(StoryGuide narrator, SectionToken stok, bool& flag1)
{
  if (narrator.checkSetting(stok.getVarName()) == stok.getVarStatus())
    flag1 = true;
  else
    flag1 = false;
}

void callElseIf(StoryGuide narrator, SectionToken stok, bool& flag1)
{
   if (flag1 == false && narrator.checkSetting(stok.getVarName()) == stok.getVarStatus())
     flag1 = true;
   else if (flag1 == true)
     flag1 = true;
}

void callElse(bool& flag1)
{
  if (flag1 == false)
    flag1 = true;
  else if (flag1 == true)
    flag1 = false;
}

void continueStory(vector<string>& links, vector<string> redirectLinks, string &passToGo, string passageRedirect, bool flag3)
{
  int op = 0;

  if (flag3 == true)
  {
    links.push_back(passageRedirect);
    passToGo = links.at(links.size()-1);
  }
  else if (links.size() == 0 && flag3 == false)
  {
    cout << "thanks for playing!" << endl;
    exit(0);
  }
  else
  {
    {
      for (int i = 0; i < links.size(); i++)
      {
        cout << i + 1 << ": " << links.at(i) << endl;
      }
      cout << "What would you like to do: " << endl;
      cin >> op;
      while (op < 0 || op > links.size())
      {
        cout << "What would you like to do: " << endl;
        cin >> op;
      }

      if (redirectLinks.at(op - 1) == " ")
        passToGo = links.at(op - 1);
      else
        passToGo = redirectLinks.at(op - 1);
    }
  }
  
}
