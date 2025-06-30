# Epoca Normal 2025 (LCDM)
### (Há de ser moderadamente parecido)


### **1.-**
A

(keep-alive muito elevado permite ao cliente não ter que estar sempre a reestabelecer ligações TCP, o que reduz a latência, e não custa assim tantos recursos, dado que um cliente não se conecta a mais de 10 paginas ao mesmo tempo a qualquer momento. No entanto o servidor que está conectado a milhares de clientes começa a sentir pressão, dado que custa (particularmente memória) manter essas ligações todas abertas)



### **2.-**
B

(Cristian tem um servidor que serve o tempo, e é considerado autoritário, quando alguem quer saber o tempo, pergunta-lhe qual é, e utiliza um algoritmo para determinar o erro dessa leitura. Berkely fala de um sistema em que vários computadores numa rede manteem um relogio pessoal, e periodicamente sincronizam com um servidor "mestre", que faz a média dos tempos, remove outliers, e aplica um reajuste a todos os relógios, formando tempo por consenso.)



### **3.-**
A

(idenpotentes e at-most-once são sinónimos, e referem-se a operações que podem ser repetidas sem causar efeitos secundários adicionais.)



### **4.-**
:(



### **5.-**
[Deu igual ao nuno]



### **6.-**
[O do Nuno é válido]



### **7.-**
[Igual ao à EN2024?]

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

É para o servidor poder descartar da sua memória o resultado da request.

O RRA (Request-Reply-Acknowledgement) consiste em o cliente fazer uma request, o servidor processar o seu request, guardar a resposta, e envia-la para o cliente. Caso o cliente não receba a resposta, não tem que obrigar o servidor a processar novamente o seu request (algo que até pode ser indesejado se o request realizou uma ação que alterou o estado do sistema, como transação bancária, e que não queremos MESMO que aconteça duas vezes em vez de uma), pedindo apenas a resposta que já tinha sido guardada, garantindo que a request é processada *at-most-once*. Depois, quando finalmente receber a resposta, o cliente envia um ack para o servidor, sinalizando que já pode descartar a resposta guardada, libertando memória.



### **9.-**
Para procurar um ficheiro no BitTorrent, é necessário ter o ficheiro *.torrent* do ficheiro que se quer. Este contem informação sobre o ficheiro como nome, tamanho, hash e o contacto do servidor tracker, que é o peer responsável por manter a lista de peers que têm partes deste ficheiro. O client comunica com o tracker, que lhe dá a lista de peers com partes do ficheiro (e depois, usando uma variedade de algoritmos, o client faz requests aos peers dessa lista, para otimizar o tempo de fetch)

Para procurar um ficheiro no Gnutella, é feito *flood-query*, onde um client faz a query aos seus vizinhos, se estes não tiverem o ficheiro replicam a query para os seus vizinhos, e por ai adiante, até a query chegar a todos os clients, ou até o ficheiro ser encontrado, passando up-stream a informação sobre quem tem o ficheiro de volta para o client original (que depois pode comunicar com o client que tem o ficheiro para o copiar)