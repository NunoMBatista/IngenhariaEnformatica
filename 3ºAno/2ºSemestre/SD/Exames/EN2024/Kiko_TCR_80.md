## Epoca Normal 2024



### **1.-**
C
(como é q conseguirias transmitir valores entre computadores se estas a passar só referencias? O computador a que o RMI está a ser aplicado n tem acesso às tuas variaveis para consultar a referencia, é preciso fazer marshalling e enviar o valor real. Para objetos java.rmi.Remote é possível passar referências, ou seja o cliente recebe um proxy do objeto remoto)



### **2.-**
C
(multicast bem mau, e o enunciado não especifica que é Reliable Multicast)



### **3.-**
D
(esta é a melhor maneira de minimizar o número de transmissões guardadas, dado que no momento em que é recebido o reply, essa passa a ser inutil, e pode ser descartada)



### **4.-**
A
(um keep-alive muito reduzido obriga a reestabelecer a ligação para continuar a comunicar, enquanto um muito longo fica a espera de receber novas comunicações, tendo que manter a ligação aberta por muito tempo, o que é mau para o desempenho do sistema)



### **5.-**

### **5.a-**

    b->c
    d->e

    Justificação: "c" acontece depois de "b", pois "c" é a recessão da mensagem enviada no momento "b", algo que só pode acontecer depois da mensagem ser enviada.



### **6.-**
[Nuno não deixa por imagem :(, deu me igual a ele]



### **7.-**

    1. - A->B   {[M]KApriv}KBpub                    , M="olá bob, tou aqui com uma vontade de te enviar ou documento, manda ai um ID unico"
    
    (Alice envia ao Bob um pedido de iniciar a transação de enviar o ficheiro, pedindo um ID unico para assinar o ficheiro.
    A mensagem está assinada com a KApriv, para o Bob ter a certeza q o request é da Alice [pode verificar a assinatura com a KApub], e toda essa package está encriptada com a KBpriv, para apenas o Bob conseguir ler a mensagem e a assinatura)


    2. - B->A   {[M]KBpriv}KApub                    , M="ID=<id>"
    
    (Bob envia o ID unico. É importante que este seja unico, para garantir repudiabilidade, e garantia contra ataques de replay. As proximas msgs têm que incluir este ID, e se for um ID desatualiado pode só ser ignorada.
    A mensagem está assinada com a KBpriv, para a Alice ter a certeza q o request é do Bob [pode verificar a assinatura com a KBpub], e toda essa package está encriptada com a KApriv, para apenas a Alice conseguir ler o ID e a assinatura)


    3. - A->B   {[D, ID]KApriv}KBpub                , D=(Documento, ou partes dele se não couber) ID=(id dado pelo Bob)
    
    (Alice envia ao Bob o documento D (ou partes do documento, se for demasiado grande), juntamente com o ID. Como o Bob acabou de dar esse ID, e ele ainda não foi usado por outro envio, trata-o como valido, defendendo contra playback.
    O documento está assinada com a KApriv, para o Bob ter a certeza q o documento vem da Alice [pode verificar a assinatura com a KApub], e toda essa package está encriptada com a KBpriv, para apenas o Bob conseguir ler a mensagem e a assinatura)

    [SE ESTAMOS A ENVIAR O "D" POR PARTES, REPETIR PASSO 3 ATÉ ENVIAR O DOCUMENTO TODO]


    4. - A->B   {[M, ID]KApriv}KBpub                , M="envio completo, valida ai" ID=(id dado pelo Bob)
    
    (Alice envia ao Bob uma confirmação de que o documento já acabou de ser enviado, e pede validação de que ele consegui o documento completo, e de que ele está legivel.
    O documento está assinada com a KApriv, para o Bob ter a certeza q o documento vem da Alice [pode verificar a assinatura com a KApub], e toda essa package está encriptada com a KBpriv, para apenas o Bob conseguir ler a mensagem e a assinatura)


    5. - B->A   {[H(D')]KBpriv}KApub                    , H(D') é a hash do documento recebido
    
    (Bob envia um Hash feito a partir do documento completo, com um algoritmo de hashing previamente acordado. Isto permite a Alice ter a certeza que o Bob recebeu o documento na sua integra e sem alterações, podendo validar o hash recebido aplicando o mesmo algoritmo de Hash a sua cópia do documento.
    A mensagem está assinada com a KBpriv, para a Alice ter a certeza q o request é do Bob [pode verificar a assinatura com a KBpub], e toda essa package está encriptada com a KApriv, para apenas a Alice conseguir ler o ID e a assinatura)



### **8.-**
O protocolo OAuth permite a Aplicações gerarem certificados de autenticação que permite a third-parties realizarem operações limitadas e controladas com o perfil de um dos seus utilizadores, sem precisar que este forneça as credenciais de login à third-party.

Deste modo, uma aplicação que queira realizar uma operação no perfil de um outro serviço (como procurar contactos, fazer uma publicação, seguir um perfil), não o faz pedindo as credenciais de login diretamente ao utilizador. Em vez disso, pede-lhe que gere uma ACCESS_TOKEN no serviço a que quer aceder, que lhe dá apenas permissões muito limitadas para realizar apenas certas operações que o utilizador consente.

Desta maneira não há problemas de a aplicação third-party que possam não ser confiaveis tenha controlo total sobre a conta do utilizador no serviço gerador de OAuth, as permissões podem ser revocadas pelo gerador de OAuth ao revocar o APP_ID/SECRET, ou pelo utilizador ao revocar ou rejeitar o ACCESS_TOKEN. Para alêm disso as passwords não possam ser roubadas, nem contas locked-out.



### **9.-**
Versão original do Gnutella, não eram usados *Super-Nodes*, quando um cliente queria fazer pesquisa, fazia uma query flooding, perguntando aos seus vizinhos diretos, que por sua vez perguntavam aos seus vizinhos, e assim sucessivamente, até que a mensagem chegasse a todos os clientes na rede. Isto era bem mau, enchendo a rede de tráfego desnecessário, ao ponto que as vezes 70% do tráfego da rede era só de pesquisas.

No rede P2P KaZaA, foi introduzido o conceito de *Super-Nodes*, que são clientes normais, promovídos a *Super-Nodes* por demonstrarem certas características (disponibilidade constante, boa largura de banda, poder computacional), e que são responsáveis por um conjunto de nodes vizinhos. Quando um cliente quer fazer uma pesquisa, ele envia a pesquisa ao seu *Super-Node*, que dá query aos seus nodes "minions", e depois faz flooding a pesquisa aos seus vizinhos *Super-Nodes*, que por sua vez fazem flooding aos seus vizinhos *Super-Nodes*, e assim sucessivamente, até que a mensagem chegue a todos os clientes na rede. Desta maneira, o tráfego de pesquisa é muito mais controlado, e não enche a rede com tráfego desnecessário, pois só os *Super-Nodes* fazem flooding, e não todos os clientes.




### **10.-**
Para conseguir este efeito geralmente são utilizados "*Brokers*", agentes intermédios, reponsáveis por receber a mensagem a ser distribuida, e garantir que todos os recipientes a recebem.

Desta maneira, o remetente consegue publicar mensagens que seram distribuidas por vários cliente que ele não sabe a localização física (conseguindo assim desacoplamento espacial) e o *Broker* garante que recipiente que não estejam online no momento recebem a publicação a que estão subscritos quando voltarem (garantindo assim desacoplamente temporal)
