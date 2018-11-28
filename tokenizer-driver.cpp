#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "commandFunctions.h"
#include "storytokenizer.hpp"

using namespace std;

int main()
{
    bool flag1 = false, flag2 = false, goToFlag = false;
    string line, story, passToGo, passageRedirect = " ", file_name;
    StoryGuide narrator;
    vector<string> passages, links, redirectLinks, goTo;
   
    cin >> file_name;
    ifstream in(file_name);

    getline(in, line);
    while (in && line != "</html>")
    {
        story += line + '\n';
        getline(in, line);
    }
    StoryTokenizer st(story);

    int pass = 0;
    while (st.hasNextPassage())
    {
     PassageToken ptok = st.nextPassage();
     narrator.setPassageMap(ptok.getName(), pass);
     passages.push_back(ptok.getText());
     pass++;
    }

    PassageTokenizer pt(passages.at(0));
    while (pt.hasNextSection())
    {
      SectionToken stok = pt.nextSection();
      switch (stok.getType())
      {
          case LINK:   callLink(stok, links, redirectLinks);  break;
          case SET:    narrator.setSetting(stok.getVarName(), stok.getVarStatus()); break;
          case GOTO:   break;
          case IF:     callIf(narrator, stok, flag1);       break;
          case ELSEIF: callElseIf(narrator,  stok, flag1);  break;
          case ELSE:   callElse(flag1);                     break;
          case BLOCK:  callBlock(stok, flag1, flag2);       break;
          case TEXT:   cout << stok.getText();              break;
          default: cout << "  Unknown token:  ";
      }
    }
    cout << endl;
    continueStory(links, redirectLinks, passToGo, passageRedirect, goToFlag);
  // Continuation of story if there are links
      while (hasLinks(links))
      {
        links.clear();
        redirectLinks.clear();
        goToFlag = false;
        PassageTokenizer pt(passages.at(narrator.findPassageIndex(passToGo)));
        while (pt.hasNextSection())
        {
          flag2 = false;
          SectionToken stok = pt.nextSection();
          switch (stok.getType())
          {
          case LINK:
            callLink(stok, links, redirectLinks);
            break;
          case SET:
          if (!goToFlag)
            narrator.setSetting(stok.getVarName(), stok.getVarStatus());
            break;
          case GOTO:
            callGoTo(stok, passageRedirect, goToFlag);
            break;
          case IF:
          if (!goToFlag)
            callIf(narrator, stok, flag1);
            break;
          case ELSEIF:
          if (!goToFlag)
            callElseIf(narrator, stok, flag1);
            break;
          case ELSE:
          if (!goToFlag)
            callElse(flag1);
            break;
          case BLOCK:
          if (!goToFlag)
            callBlock(stok, flag1, flag2);
            break;
          case TEXT:
          if (!goToFlag)
            cout << stok.getText();
            break;
          default: cout << "  Unknown token:  ";
          }
        }
        cout << endl;
        continueStory(links, redirectLinks, passToGo, passageRedirect, goToFlag);
  }
    return 0;
}
