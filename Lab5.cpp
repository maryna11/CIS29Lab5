/*
 Lab 5: Huffman Encoding
 11 June 2016
 Lab5.cpp
 Purpose: Use a variety of STL structure to use Huffman Encoding
 */

// #ifndef Lab5_H_
// #define Lab5_H_
// #include "Lab5.h"

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <map> // tree
#include <vector>
#include <queue> // priority_queue
#include <regex>
#include <list>
#include <iterator>
#include <memory>
#include <functional> // ref()
#include <cstddef> // Null
using namespace std;

class XMLnode {
private:
    char tag;
    int value; // frequency
    vector<XMLnode> data;
public:
    XMLnode() : tag(' '), value(0) {}
    ~XMLnode() {}
    char getTag() {return tag;}
    void setTag(char t) {tag = t;}
    int getValue() {return value;}
    void setValue(int v) {value = v;}
};

class XMLprocessor {
private:
    map<char,int> frequencyMap;
    vector<XMLnode> vList; // frequency
    list<XMLnode> linkedList;
    // int findFrequency(char v) {return frequencyMap[v];}
    bool static compare(XMLnode& a, XMLnode& b); // other option: operator overload
public:
    XMLprocessor() {}
    ~XMLprocessor() {}
    // void processHeader(string h); // (optional: if header exists)
    map<char,int> getFrequencyMap() {return frequencyMap;}
    void processLine(string l);
    vector<string> regexString(string text, string split);
    vector<XMLnode> onInputDone(); // Sorts the list
};

// Abstract Class // Base Class
class Node {
public:
    // No Constructor!
    virtual ~Node() {}
    virtual bool isLeafNode() = 0;
};

// Derived Class
class Branch: public Node {
    Node* left;
    Node* right;
public:
    Branch() : left(NULL), right(NULL) {} // Constructor
    ~Branch() {}
    Node* getLeft() {return left;}
    Node* getRight() {return right;}
    void setLeft(Node* l) {left = l;} // left = 0
    void setRight(Node* r) {right = r;} // right = 1
    bool isLeafNode() override {return false;} //  Explicit Overriding
};

// Derived Class
class Leaf: public Node {
    char symbol;
public:
    Leaf() : symbol(' ') {} // Constructor
    ~Leaf() {}
    char getSymbol() {return symbol;}
    void setSymbol(char s) {symbol = s;}
    bool isLeafNode() override {return true;} // Explicit Overriding
};

// map tree
class TreeBuilder {
private:
    map<const Node*,int> tree;
    map<char,int> frequencies;
    Branch* root; // typedef struct Node* Nodeptr;
    Branch* current;
public:
    TreeBuilder() : root(new Branch()) {} // root(NULL), root(*(tree.begin()))
    ~TreeBuilder() {}
    map<const Node*,int> getTree() {return tree;}
    void setTree(map<const Node*,int> t) {tree = t;}
    void setFrequencies(map<char,int> f) {frequencies = f;}
    void add(char data) {if(frequencies.count(data)) addNode(root,data); }
    void addNode(Branch* root, char data);
    string encrypt(string s);
    string getBits(char c) {if(frequencies.count(c)) return getPattern(root,c); else return "";}
    string getPattern(Branch* rt, char c);
    string outputIntoFile(ofstream& fout, string s);
    string parse(string s);
};

class Decrypt {
private:
    char letter;
    int bitPattern;
    TreeBuilder decryptionTree; // same tree
public:
    Decrypt() : letter(' '), bitPattern(0) {}
    ~Decrypt() {}
    char getLetter() {return letter;}
    void setLetter(char l) {letter = l;}
    int getBitPattern() {return bitPattern;}
    void setBitPattern(int b) {bitPattern = b;}
    void setDecryptionTree(TreeBuilder t) {decryptionTree = t;}
    void outputIntoFile(ofstream& fout, string s);
};

// ***XMLprocessor Functions***

// Sorts the list from biggest to smallest
bool XMLprocessor::compare(XMLnode& a, XMLnode& b)
{
    int s1;
    int s2;
    
    s1 = a.getValue();
    s2 = b.getValue();
    /*
    if(s1  s2)
        return -1;
    else if(s1 == s2)
        return 0;
    else
     */
    return s1 > s2;
}

// Processes a line of XML
void XMLprocessor::processLine(string l)
{
    vector<string> matches;
    
    // if the line contains both the begin and end tag, push_back the data into a vector
    if(regex_match(l, regex(R"(<.>.*<\\.*>)"))) // Raw
    {
        matches = regexString(l, "[^(<|>)]+");
        XMLnode temp;
        temp.setTag(matches[0][0]); // --> char symbol
        temp.setValue(atoi(matches[1].c_str())); // --> int frequency
        frequencyMap[matches[0][0]] = atoi(matches[1].c_str());
        linkedList.push_back(temp);
    }
}

// Returns a vector of matches
vector<string> XMLprocessor::regexString(string text, string split)
{
    vector<string> container;
    sregex_token_iterator end; // size is ZERO when created
    regex pattern(split);
    for (sregex_token_iterator pos(text.begin(), text.end(), pattern); pos != end; ++pos)
    {
        container.push_back(*pos);
    }
    return container;
}

// Sorts the list
// Put the list into a vector
vector<XMLnode> XMLprocessor::onInputDone()
{
    // 2. After processing all the nodes, sort the data using the STL List Sort routine.
    linkedList.sort(compare); // linkedList.sort(this->compare);
    // Put the list into a vector
    for(list<XMLnode>::iterator i = linkedList.begin(); i != linkedList.end(); ++i)
    {
        vList.push_back(*i); // priority_queue
        // cout << "v: " << i->getValue() << endl;
        // cout << "t: " << i->getTag() << endl;
    }
    return vList;
}

// ***TreeBuilder Functions***

// Recursively builds the tree
void TreeBuilder::addNode(Branch* root, char data)
{
    if(root->getLeft() == NULL)
    {
        Branch* b = new Branch;
        Leaf* l = new Leaf;
        l->setSymbol(data);
        b->setLeft(l);
        root->setLeft(b);
        return;
    }
    else // != NULL
    {
        Branch* b = dynamic_cast<Branch*>(root->getLeft());
        if(b->getRight() == NULL)
        {
            Leaf* l = new Leaf;
            l->setSymbol(data);
            b->setRight(l);
            return;
        }
    }
    if(root->getRight() == NULL)
    {
        Branch* b = new Branch;
        Leaf* l = new Leaf;
        l->setSymbol(data);
        b->setLeft(l);
        root->setRight(b);
        return;
    }
    Branch* b = dynamic_cast<Branch*>(root->getRight());
    if(b->getLeft()->isLeafNode())
    {
        if(b->getRight() == NULL)
        {
            Leaf* l = new Leaf;
            l->setSymbol(data);
            b->setRight(l);
            return;
        }
        else
        {
            Branch* br = new Branch;
            br->setLeft(b);
            root->setRight(br);
            addNode(br, data); // recursive call
            return;
        }
    }
    else
    {
        addNode(b, data);
        return;
    }
}

string TreeBuilder::getPattern(Branch* rt, char c)
{
    Branch* b;
    b = dynamic_cast<Branch*>(rt->getLeft());
    if(dynamic_cast<Leaf*>(b->getLeft())->getSymbol() == c)
    {
        return "00";
    }
    if(dynamic_cast<Leaf*>(b->getRight())->getSymbol() == c)
    {
        return "01";
    }
    b = dynamic_cast<Branch*>(rt->getRight());
    if(b->getLeft()->isLeafNode())
    {
        if(dynamic_cast<Leaf*>(b->getLeft())->getSymbol() == c)
        {
            return "10";
        }
        if(dynamic_cast<Leaf*>(b->getRight())->getSymbol() == c)
        {
            return "11";
        }
    }
    
    return "1" + getPattern(b, c);
}

// Turns string of bitsets into ASCII
string TreeBuilder::encrypt(string s)
{
    // string debug;
    bitset<8> symbol;
    string pattern;
    string output;
    for(int i = 0; i < s.size(); i++)
    {
        char c = s[i];
        pattern += getBits(c);
        // debug += getBits(c);
            while ( pattern.length() >= 8)
            {
                symbol = bitset<8>(pattern.substr(0, 8));
                char i = (char)symbol.to_ulong();
                output += i;
                pattern.erase(0, 8);
            }
    }
    // fillers
    if (pattern.length() > 0) {
        string strWhiteSpace = getBits(' ');
        size_t curSpaceBit = 0;
        while (pattern.length() < 8)
        {
            pattern += strWhiteSpace[ curSpaceBit++ % strWhiteSpace.length() ];
        }
        symbol = bitset<8>(pattern);
        char i = (char)symbol.to_ulong();
        output +=i;
    }
    // cout << "Debug: " << debug <<endl;
    return output;
}

string TreeBuilder::outputIntoFile(ofstream& fout, string s)
{
    string out = encrypt(s);
    fout << out;
    // cout << out;
    return out;
}

string TreeBuilder::parse(string s)
{
    // string debug;
    current = root;
    Node* n = nullptr;
    bitset<8> symbol;
    bitset<8> bits;
    string output;
    for(int i = 0; i < s.size(); i++)
    {
        char c = s[i];
        bits = bitset<8>(c);
        // debug += bitset<8>(c).to_string();
        for (size_t i = 0; i < 8; ++i)
        {
            if( current != NULL)
            {
                if(bits[7-i] == 0)
                {
                    n = current->getLeft();
                }
                else
                    n = current->getRight();
            }
            if( n != NULL)
            {
            if(n->isLeafNode())
            {
                output += dynamic_cast<Leaf*>(n)->getSymbol();
                current = root;
                //cout << output;
            }
            else
            {
                current = dynamic_cast<Branch*>(n);
            }
            }
        }
    }
    // cout << "Parse: " << debug << endl;
    return output;
}

void Decrypt::outputIntoFile(ofstream& fout, string s)
{
    string out = decryptionTree.parse(s);
    fout << out;
    cout << out;
}

// #endif

// programmed by
void signature()
{
    cout << " " << endl;
    cout << "Programmed by: " << endl;
    cout << "Maryna" << endl;
    cout << " " << endl;
}

///*
 // Memory Leak Check: Visual Studio
 void enableDebug(bool bvalue)
 {
 if (!bvalue) return;
 
 int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
 
 // Turn on leak-checking bit.
 tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
 
 // Turn off CRT block checking bit.
 tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;
 
 // Set flag to the new value.
 _CrtSetDbgFlag(tmpFlag);
 }
 //*/

int main() {
    ifstream fin;
    ifstream speechFile;
    ofstream fout;
    ofstream decryptedSpeech;
    string line;
    string speech;
    string compressed;
    vector<XMLnode> v;
    XMLprocessor pr;
    TreeBuilder tree;
    Decrypt dc;
    
    // Memory Leak Check: Visual Studio
    enableDebug(true);
    
    // Open and read file
    speechFile.open("Speech.txt");
    if (speechFile.fail())
    {
        cout << "Speech file failed to open!\n";
        exit(-1);
    }
    
    // Process File
    while (!speechFile.eof())
    {
        getline(speechFile, line);
        speech +=line;
    }
    
    // Close input file
    speechFile.close();
    cout << " " << endl;
    cout << "Speech File processing completed." << endl;
    cout << " " << endl;
    
    // Open and read file
    fin.open("Frequencies.txt");
    if (fin.fail())
    {
        cout << "Frequencies file failed to open!\n";
        exit(-1);
    }
    
    // Process File
    while (!fin.eof())
    {
        getline(fin, line);
        pr.processLine(line);
    }
    
    // Sort the Frequencies
    v = pr.onInputDone();
    tree.setFrequencies(pr.getFrequencyMap());
    for(vector<XMLnode>::iterator i = v.begin(); i != v.end(); ++i)
    {
        tree.add(i->getTag());
    }
    
    // Close input file
    fin.close();
    cout << " " << endl;
    cout << "Frequency File processing completed." << endl;
    cout << " " << endl;
    
    fout.open("b.txt"); // ("b.bin", ios::binary)
    if (fout.fail())
    {
        cout << "Output file failed to open!\n";
        exit(-1);
    }

    compressed = tree.outputIntoFile(fout, speech); // instead of file into file
    // Close output file
    fout.close();
    // Decrypt
    dc.setDecryptionTree(tree);
    
    decryptedSpeech.open("s.txt");
    if (decryptedSpeech.fail())
    {
        cout << "Output file failed to open!\n";
        exit(-1);
    }
    dc.outputIntoFile(decryptedSpeech, compressed);
    
    // Close output file
    decryptedSpeech.close();
    cout << " " << endl;
    cout << "S File processing completed." << endl;
    cout << " " << endl;
    
    signature(); // Pogrammed by
    
    return 0;
} // main

/* Testing:
 <7>1<\7><M>1<\M><D>1<\D><2>1<\2><6>1<\6><P>1<\P><J>2<\J><B>2<\B><;>2<\;><O>2<\O><F>3<\F><C>3<\C><R>4<\R><4>4<\4><8>4<\8><K>5<\K><3>5<\3><G>5<\G><9>5<\9><1>6<\1><z>6<\z><5>6<\5><->6<\-><?>8<\?><q>8<\q><'>9<\'><N>10<\N><x>11<\x><0>12<\0><E>16<\E><j>19<\j><A>20<\A><:>21<\:><U>23<\U><W>28<\W><S>35<\S><I>45<\I><k>51<\k><T>69<\T><v>120<\v><,>146<\,><.>147<\.><b>148<\b><g>192<\g><y>209<\y><w>249<\w><m>269<\m><u>330<\u><p>338<\p><c>345<\c><f>391<\f><l>483<\l><d>486<\d><h>633<\h><r>806<\r><s>848<\s><i>861<\i><n>953<\n><a>968<\a><o>1118<\o><t>1254<\t><e>1643<\e>< >2891<\ >
 File processing completed.
 
 
 Programmed by:
 Maryna
 
 Program ended with exit code: 0
 ...
 File processing completed.
 
 \377\377\377\371\347\376\237\377\377\377\336=\347\307g\377\377\377\377\236\351\377\367\370\347\377\257\237\3743~\377\377\377y\331\315\377\277\336w
 ...
 8ms,Fsniifern, 4iw oSiepAedtmaelreRh nntEeshnnea Oe0tedl o yttoe; wsfenspWehoaaaeln atostoathorhe0 sdtiooa nn oeui n eocieWisre vt lot nfertyeiiitegw-ecseWisdtofi tooesronyou t hiosroofeie gUaeoandararuahre niogo  od uhah dae,laotsgeenonoo  ehereyteichit eOosphiiteotdttoeaeicehdioenrttstt htoaelhuft n,eheohasftfnyoieg aitreapeoneeanijetrolmtes vt netdotoe cfeinmh da ,et ehcohawf oideto eyoneieree  neircierohtt haomietaisnaohtt rdioih t hiieeaneirtclirotablee,e,e o eoooae aur lett oaininvtsatetdltau  ibdtnaeetolacueT n i hieaonnlseoto esrfnnabiernertaur  ieedo9lre pteU oltt eeeaepwt .i etrh i siooouih edpten yoIue Trci nmteaehohAee  iosiro hwhosi iistoeuet tth isoefsteoteaeoii egld nonpdoeean tfdc boenon.
 ...
 2rTPresidetab eOr. Sprker, atf Orstinguished Mepeyers of the Cot.reahAI stay orthiserctrup euith t ehnseeof deep Tyilioy and :rt pride -- hupeality sn thawakaofetul great Wyeribrarchstecos  f eeurehistory wu hnve stoodeher  becore mDepride ia the reflectson tiit thisecorfy y rogi.atiS deyette represeddTynn laeyerty in tio purest deerm m a devilreg ere nr  dontered thaupes teod aspirntsons teod fastfofetc entire humteoerterob I ree not stanr eaoi tten adUcnte f d atb uaie aenan 
 ...
 
 Speech File processing completed.
 
 
 Frequency File processing completed.
 
 Mr. President, Mr. Speaker, and Distinguished Members of the Congress:I stand on this rostrum with a sense of deep humility and great pride -- humility in the wake of those great American architects of our history who have stood here before me; pride in the reflection that this forum of legislative debate represents human liberty in the purest form yet devised. ere are centered the hopes and aspirations and faith of the entire human race. I do not stand here as advocate for any partisan cause, for the issues are fundamental and reach quite beyond the realm of partisan consideration. They must be resolved on the highest plane of national interest if our course is to prove sound and our future protected. I trust, therefore, that you will do me the justice of receiving that which I have to say as solely expressing the considered viewpoint of a fellow American.I address you with neither rancor nor bitterness in the fading twilight of life, with but one purpose in mind: to serve my country. The issues are global and so interlocked that to consider the problems of one sector, oblivious to those of another, is but to court disaster for the whole. While Asia is commonly referred to as the Gateway to Europe, it is no less true that Europe is the Gateway to Asia, and the broad influence of the one cannot fail to have its impact upon the other. There are those who claim our strength is inadequate to protect on both fronts, that we cannot divide our effort. I can think of no greater expression of defeatism. If a potential enemy can divide his strength on two fronts, it is for us to counter his effort. The Communist threat is a global one. Its successful advance in one sector threatens the destruction of every other sector. ou can not appease or otherwise surrender to communism in Asia without simultaneously undermining our efforts to halt its advance in Europe.Beyond pointing out these general truisms, I shall confine my discussion to the general areas of Asia. Before one may objectively assess the situation now existing there, he must comprehend something of Asia's past and the revolutionary changes which have marked her course up to the present. ong exploited by the so-called colonial powers, with little opportunity to achieve any degree of social justice, individual dignity, or a higher standard of life such as guided our own noble administration in the Philippines, the peoples of Asia found their opportunity in the war just past to throw off the shackles of colonialism and now see the dawn of new opportunity, a heretofore unfelt dignity, and the self-respect of political freedom.Mustering half of the earth's population, and 60 percent of its natural resources these peoples are rapidly consolidating a new force, both moral and material, with which to raise the living standard and erect adaptations of the design of modern progress to their own distinct cultural environments. Whether one adheres to the concept of colonization or not, this is the direction of Asian progress and it may not be stopped. It is a corollary to the shift of the world economic frontiers as the whole epicenter of world affairs rotates back toward the area whence it started.In this situation, it becomes vital that our own country orient its policies in consonance with this basic evolutionary condition rather than pursue a course blind to the reality that the colonial era is now past and the Asian peoples covet the right to shape their own free destiny. What they seek now is friendly guidance, understanding, and support -- not imperious direction -- the dignity of equality and not the shame of subjugation. Their pre-war standard of life, pitifully low, is infinitely lower now in the devastation left in war's wake. World ideologies play little part in Asian thinking and are little understood. What the peoples strive for is the opportunity for a little more food in their stomachs, a little better clothing on their backs, a little firmer roof over their heads, and the realization of the normal nationalist urge for political freedom. These political-social conditions have but an indirect bearing upon our own national security, but do form a backdrop to contemporary planning which must be thoughtfully considered if we are to avoid the pitfalls of unrealism.Of more direct and immediate bearing upon our national security are the changes wrought in the strategic potential of the Pacific Ocean in the course of the past war. Prior thereto the western strategic frontier of the United States lay on the littoral line of the Americas, with an exposed island salient extending out through awaii, Midway, and Guam to the Philippines. That salient proved not an outpost of strength but an avenue of weakness along which the enemy could and did attack.The Pacific was a potential area of advance for any predatory force intent upon striking at the bordering land areas. All this was changed by our Pacific victory. Our strategic frontier then shifted to embrace the entire Pacific Ocean, which became a vast moat to protect us as long as we held it. Indeed, it acts as a protective shield for all of the Americas and all free lands of the Pacific Ocean area. We control it to the shores of Asia by a chain of islands extending in an arc from the Aleutians to the Mariannas held by us and our free allies. From this island chain we can dominate with sea and air power every Asiatic port from ladivostok to Singapore -- with sea and air power every port, as I said, from ladivostok to Singapore -- and prevent any hostile movement into the Pacific.Any predatory attack from Asia must be an amphibious effort. No amphibious force can be successful without control of the sea lanes and the air over those lanes in its avenue of advance. With naval and air supremacy and modest ground elements to defend bases, any major attack from continental Asia toward us or our friends in the Pacific would be doomed to failure.Under such conditions, the Pacific no longer represents menacing avenues of approach for a prospective invader. It assumes, instead, the friendly aspect of a peaceful lake. Our line of defense is a natural one and can be maintained with a minimum of military effort and expense. It envisions no attack against anyone, nor does it provide the bastions essential for offensive operations, but properly maintained, would be an invincible defense against aggression. The holding of this littoral defense line in the western Pacific is entirely dependent upon holding all segments thereof; for any major breach of that line by an unfriendly power would render vulnerable to determined attack every other major segment.This is a military estimate as to which I have yet to find a military leader who will take exception. For that reason, I have strongly recommended in the past, as a matter of military urgency, that under no circumstances must Formosa fall under Communist control. Such an eventuality would at once threaten the freedom of the Philippines and the loss of Japan and might well force our western frontier back to the coast of California, Oregon and Washington.To understand the changes which now appear upon the Chinese mainland, one must understand the changes in Chinese character and culture over the past 50 years. China, up to 50 years ago, was completely non-homogenous, being compartmented into groups divided against each other. The war-making tendency was almost non-existent, as they still followed the tenets of the Confucian ideal of pacifist culture. At the turn of the century, under the regime of Chang Tso in, efforts toward greater homogeneity produced the start of a nationalist urge. This was further and more successfully developed under the leadership of Chiang Kai-Shek, but has been brought to its greatest fruition under the present regime to the point that it has now taken on the character of a united nationalism of increasingly dominant, aggressive tendencies.Through these past 50 years the Chinese people have thus become militarized in their concepts and in their ideals. They now constitute excellent soldiers, with competent staffs and commanders. This has produced a new and dominant power in Asia, which, for its own purposes, is allied with Soviet Russia but which in its own concepts and methods has become aggressively imperialistic, with a lust for expansion and increased power normal to this type of imperialism.There is little of the ideological concept either one way or another in the Chinese make-up. The standard of living is so low and the capital accumulation has been so thoroughly dissipated by war that the masses are desperate and eager to follow any leadership which seems to promise the alleviation of local stringencies.I have from the beginning believed that the Chinese Communists' support of the North Koreans was the dominant one. Their interests are, at present, parallel with those of the Soviet. But I believe that the aggressiveness recently displayed not only in Korea but also in Indo-China and Tibet and pointing potentially toward the South reflects predominantly the same lust for the expansion of power which has animated every would-be conqueror since the beginning of time.The Japanese people, since the war, have undergone the greatest reformation recorded in modern history. With a commendable will, eagerness to learn, and marked capacity to understand, they have, from the ashes left in war's wake, erected in Japan an edifice dedicated to the supremacy of individual liberty and personal dignity; and in the ensuing process there has been created a truly representative government committed to the advance of political morality, freedom of economic enterprise, and social justice.   Politically, economically, and socially Japan is now abreast of many free nations of the earth and will not again fail the universal trust. That it may be counted upon to wield a profoundly beneficial influence over the course of events in Asia is attested by the magnificent manner in which the Japanese people have met the recent challenge of war, unrest, and confusion surrounding them from the outside and checked communism within their own frontiers without the slightest slackening in their forward progress. I sent all four of our occupation divisions to the Korean battlefront without the slightest qualms as to the effect of the resulting power vacuum upon Japan. The results fully justified my faith. I know of no nation more serene, orderly, and industrious, nor in which higher hopes can be entertained for future constructive service in the advance of the human race.Of our former ward, the Philippines, we can look forward in confidence that the existing unrest will be corrected and a strong and healthy nation will grow in the longer aftermath of war's terrible destructiveness. We must be patient and understanding and never fail them -- as in our hour of need, they did not fail us. A Christian nation, the Philippines stand as a mighty bulwark of Christianity in the Far East, and its capacity for high moral leadership in Asia is unlimited.  On Formosa, the government of the Republic of China has had the opportunity to refute by action much of the malicious gossip which so undermined the strength of its leadership on the Chinese mainland. The Formosan people are receiving a just and enlightened administration with majority representation on the organs of government, and politically, economically, and socially they appear to be advancing along sound and constructive lines.With this brief insight into the surrounding areas, I now turn to the Korean conflict. While I was not consulted prior to the President's decision to intervene in support of the Republic of Korea, that decision from a military standpoint, proved a sound one, as we -- as I said, proved a sound one, as we hurled back the invader and decimated his forces. Our victory was complete, and our objectives within reach, when Red China intervened with numerically superior ground forces.This created a new war and an entirely new situation, a situation not contemplated when our forces were committed against the North Korean invaders; a situation which called for new decisions in the diplomatic sphere to permit the realistic adjustment of military strategy.Such decisions have not been forthcoming.While no man in his right mind would advocate sending our ground forces into continental China, and such was never given a thought, the new situation did urgently demand a drastic revision of strategic planning if our political aim was to defeat this new enemy as we had defeated the old.Apart from the military need, as I saw It, to neutralize the sanctuary protection given the enemy north of the alu, I felt that military necessity in the conduct of the war made necessary: first the intensification of our economic blockade against China; two the imposition of a naval blockade against the China coast; three removal of restrictions on air reconnaissance of China's coastal areas and of Manchuria;  four removal of restrictions on the forces of the Republic of China on Formosa, with logistical support to contribute to their effective operations against the common enemy.For entertaining these views, all professionally designed to support our forces committed to Korea and bring hostilities to an end with the least possible delay and at a saving of countless American and allied lives, I have been severely criticized in lay circles, principally abroad, despite my understanding that from a military standpoint the above views have been fully shared in the past by practically every military leader concerned with the Korean campaign, including our own Joint Chiefs of Staff.   I called for reinforcements but was informed that reinforcements were not available. I made clear that if not permitted to destroy the enemy built-up bases north of the alu, if not permitted to utilize the friendly Chinese Force of some 600,000 men on Formosa, if not permitted to blockade the China coast to prevent the Chinese Reds from getting succor from without, and if there were to be no hope of major reinforcements, the position of the command from the military standpoint forbade victory.We could hold in Korea by constant maneuver and in an approximate area where our supply line advantages were in balance with the supply line disadvantages of the enemy, but we could hope at best for only an indecisive campaign with its terrible and constant attrition upon our forces if the enemy utilized its full military potential. I have constantly called for the new political decisions essential to a solution.Efforts have been made to distort my position. It has been said, in effect, that I was a warmonger. Nothing could be further from the truth. I know war as few other men now living know it, and nothing to me is more revolting. I have long advocated its complete abolition, as its very destructiveness on both friend and foe has rendered it useless as a means of settling international disputes. Indeed, on the second day of September, nineteen hundred and forty-five, just following the surrender of the Japanese nation on the Battleship Missouri, I formally cautioned as follows:Men since the beginning of time have sought peace. arious methods through the ages have been attempted to devise an international process to prevent or settle disputes between nations. From the very start workable methods were found in so far as individual citizens were concerned, but the mechanics of an instrumentality of larger international scope have never been successful. Military alliances, balances of power, eagues of Nations, all in turn failed, leaving the only path to be by way of the crucible of war. The utter  destructiveness of war now blocks out this alternative. We have had our last chance. If we will not devise some greater and more equitable system, Armageddon will be at our door. The problem basically is theological and involves a spiritual recrudescence and improvement of human character that will synchronize with our almost matchless advances in science, art, literature, and all material and cultural developments of the past 2000 years. It must be of the spirit if we are to save the flesh.But once war is forced upon us, there is no other alternative than to apply every available means to bring it to a swift end.War's very object is victory, not prolonged indecision.In war there is no substitute for victory.There are some who, for varying reasons, would appease Red China. They are blind to history's clear lesson, for history teaches with unmistakable emphasis that appeasement but begets new and bloodier war. It points to no single instance where this end has justified that means, where appeasement has led to more than a sham peace. ike blackmail, it lays the basis for new and successively greater demands until, as in blackmail, violence becomes the only other alternative.Why, my soldiers asked of me, surrender military advantages to an enemy in the field? I could not answer.Some may say: to avoid spread of the conflict into an all-out war with China; others, to avoid Soviet intervention. Neither explanation seems valid, for China is already engaging with the maximum power it can commit, and the Soviet will not necessarily mesh its actions with our moves. ike a cobra, any new enemy will more likely strike whenever it feels that the relativity in military or other potential is in its favor on a world-wide basis.The tragedy of Korea is further heightened by the fact that its military action is confined to its territorial limits. It condemns that nation, which it is our purpose to save, to suffer the devastating impact of full naval and air bombardment while the enemy's sanctuaries are fully protected from such attack and devastation.Of the nations of the world, Korea alone, up to now, is the sole one which has risked its all against communism. The magnificence of the courage and fortitude of the Korean people defies description.  They have chosen to risk death rather than slavery. Their last words to me were: Don't scuttle the PacificI have just left your fighting sons in Korea. They have met all tests there, and I can report to you without reservation that they are splendid in every way.It was my constant effort to preserve them and end this savage conflict honorably and with the least loss of time and a minimum sacrifice of life. Its growing bloodshed has caused me the deepest anguish and anxiety.  Those gallant men will remain often in my thoughts and in my prayers always.I am closing my 52 years of military service. When I joined the Army, even before the turn of the century, it was the fulfillment of all of my boyish hopes and dreams. The world has turned over many times since I took the oath on the plain at West Point, and the hopes and dreams have long since vanished, but I still remember the refrain of one of the most popular barrack ballads of that day which proclaimed most proudly that old soldiers never die; they just fade away.And like the old soldier of that ballad, I now close my military career and just fade away, an old soldier who tried to do his duty as God gave him the light to see that duty.
 S File processing completed.
 
 
 Programmed by:
 Maryna
 
 
 Program ended with exit code: 0
 */
