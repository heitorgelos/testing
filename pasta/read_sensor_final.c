#include <stdio.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//Definição de pinos 
#define sda_port  21
#define scl_port  22

//Definição das características do mestre
#define pullup 1
#define talk_freq 100000
#define master_port 0
#define buffer_tx 0
#define buffer_rx 0

//Definição das características dos escravos (sensores de temperatura na rede)
#define address_dev1 0x3A
#define address_dev2 0x4A
#define address_dev3 0x5A
#define address_dev4 0x6A

#define   addres_reg 0x07    // Registrador que armazena a temperatura objeto em cada um dos sensores

//Configuração do mestre do barramento (este esp32) (pronto pra se comunicar após ela)
void i2c_master_config_init(){
    i2c_config_t setup = {
         .mode = I2C_MODE_MASTER,
         .sda_io_num = sda_port,
         .scl_io_num = scl_port,
         .sda_pullup_en = GPIO_PULLUP_DISABLE,
         .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = talk_freq,
        .clk_flags = 0,  
    };

    i2c_param_config(master_port,&setup);

    i2c_driver_install(master_port,setup.mode,buffer_rx,buffer_tx,0);

}

//Função que faz a requisição do dado lido pelo sensor
float i2c_request_tempC(uint8_t address_dev){

    uint8_t reg=addres_reg;
    uint16_t temp_bruta;
    float temp_celsius;
    uint8_t data[3]; //2 bytes de dados e 1 de crc (sensor tem conversor ADC de 17bits!)

     esp_err_t reading = i2c_master_write_read_device( //Read write chama sensor e pega dados instantâneamente
        master_port, //Porta do mestre
        address_dev, //Endereço do dispositivo
        &reg, //Endereço do registrador
        sizeof(reg),
        data, //Armazenamento de dados lidos
        3, //Numero de bytes lidos
        1000/portTICK_PERIOD_MS); //Frequência de leitura

        if(reading != ESP_OK){
            return 10000.00;
        }
    
        temp_bruta = (data[1]<<8)| data[0];
        temp_celsius = temp_bruta*0.02 -273.15; //Advinda do data sheet
        return temp_celsius;
}



void app_main(void)
{
    float temperature1,temperature2,temperature3,temperature4; 
    i2c_master_config_init();

    //Lê dados continuamente
    while(1){
        temperature1=i2c_request_tempC(address_dev1);
        temperature2=i2c_request_tempC(address_dev2);
        temperature3=i2c_request_tempC(address_dev3);
        temperature4=i2c_request_tempC(address_dev4);

        if (temperature1 == 10000.0f) 
            printf("Erro ao ler sensor 1\n");
        else 
            printf("Sensor 1: %.2f °C\n", temperature1);

        if (temperature2 == 10000.0f)
            printf("Erro ao ler sensor 2\n");
        else 
            printf("Sensor 2: %.2f °C\n", temperature2);

        if (temperature3 == 10000.0f) 
            printf("Erro ao ler sensor 3\n");
        else 
            printf("Sensor 3: %.2f °C\n", temperature3);

        if (temperature4 == 10000.0f) 
            printf("Erro ao ler sensor 4\n");
        else 
            printf("Sensor 4: %.2f °C\n", temperature4);

        vTaskDelay(500/portTICK_PERIOD_MS); //Delay de 500ms entre cada leitura dos sensores

    }
}
