# Exibindo o MAC

Antes de subir o código de conexão ESPNOW, deve-se exibir o endereço MAC do receptor. Para isso, siga os passos abaixo:

1. Faça upload do arquivo `exibeMAC.ino` para o ESP32.
2. Após subir o código, reinicie o ESP32.
3. Anote o endereço MAC que aparecerá no monitor serial.

Pronto! Agora você tem o endereço MAC do receptor.

# Subindo os códigos de conexão

Com o endereço MAC do receptor em mãos, insira-o no arquivo `CodTransmissor.ino` e faça o upload dos arquivos `codReceptor.ino` e `codTransmissor.ino`