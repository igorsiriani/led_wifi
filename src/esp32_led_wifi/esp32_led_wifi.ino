wo#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <WiFi.h> //funcionalidades de conexão wifi, como AP e WebServer

//Crendenciais do ponto de acesso
const char *ssid = "BDAG21";
const char *senha = "bdag2019";
WiFiServer server(80); //Porta padrão 80

#define NUM_DISPOSITIVOS 3

bool state_lamp = false;

void run_html(WiFiClient client){ 
   String html_content = \
"<!DOCTYPE html><html>" \
    "<head><style media='screen' type='text/css'>" \
            "html{margin:50px auto;text-align:center;}" \
            "button{width:200px;font-size:80px;background-color:rgb(138, 138, 255);border: 0px;border-radius: 7px;}" \
            "button:hover,button:active{background-color: rgb(88, 88, 187);}" \
            ".nomes{display: block;position: absolute; left: 5%;top: 95%;font-size:20px;}" \
    "</style></head>" \
    "<body>" \
        "<h1 style='font-size:90px'>ACIONAMENTO LED</h1>" \
        "<p><a href='/LED_ON'>" \
            "<button>ON</button>" \
        "</a></p>" \
        "<p><a href='/LED_OFF'>" \
            "<button>OFF</button>" \
        "</a></p>" \
        "<a class='nomes'>Gilberto Filipe - Iasmine Yonekura - Igor Siriani - Tales Gava - Vinicius Alves</a>" \
    "</body>" \
"</html>";
    client.println(html_content);
}

void setup(){
    //Configura pino conectado ao LED como saída
    pinMode(2, OUTPUT); 
    //Inicializa serial
    Serial.begin(115200);
    Serial.println();

    //Configura ESP no modo AP
    Serial.printf("Configurando ponto de acesso '%s'\n", ssid);
    WiFi.softAP(ssid, senha);
    server.begin();
    Serial.println("Configuração conluída");
    Serial.println(WiFi.localIP());
    
}

void loop() {
    //Verifica se existe algum client na rede 
    WiFiClient client = server.available();   
    //Caso positivo, imprime "New Client" no monitor
//    Serial.println("aaaiii");
    if (client){                              
        Serial.println("New Client");  
        //Enquanto client conectado, verifica se existem bytes a serem lidos     
        //e concatena os bytes recebidos na String cabecalho
        String cabecalho;    
        while (client.connected()){     
            if (client.available()){
                cabecalho += (char)client.read();  
                //Se receber nova linha em branco, encerrou leitura dos dados
                if (cabecalho.endsWith("\n\r\n")){
                    Serial.println(cabecalho); //imprime cabeçalhos http recebidos
                    // iniciamos a resposta http com o código OK(200), 
                    // o tipo de conteúdo a ser enviado e tipo de conexão.
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type:text/html");
                    client.println("Connection: close");
                    client.println();
                    // INSIRA AQUI SUA APLICAÇÃO

                    //envia ao client conteúdo HTML
                    run_html(client);

                      if(cabecalho.indexOf("GET /LED_ON")>= 0){
                          state_lamp = true;
                          digitalWrite(2, true);
                      }else if(cabecalho.indexOf("GET /LED_OFF")>= 0){
                          state_lamp = false;
                          digitalWrite(2, false);
                      }
                                  
                    break; //sai do while loop
                }
            }
        }
        cabecalho = ""; //ao encerrar conexão, limpa variável cabecalho
        client.flush(); client.stop();
        Serial.println("Client desconectado."); Serial.println();
    }
}
