# CESE - SOPG TP 2

## Introducción

El presente proyecto es uno de las cuatro softwares de un sistema de domótica para el manejo de 3 luces. El sistema permite controlar las luces (emuladas con leds) desde una pagina web y desde los pulsadores del
controlador.  
El proyecto en cuestión se llama *Serial Service*, y es el encargado de recibir tramas del *Serial Emulator* y enviarlos al *Interface Service*. Al mismo tiempo, recibirá tramas de *Interface Service* y las enviará al *Serial Emulator*.  
El *Interfce Service* realiza un función similar actuando como interfaz entre el *Web Service* y el *Serial Service*.  

## Compilación

Suponiendo que el repositorio fue clonado en '~', los siguientes comandos ejecutados desde la raíz compilaran el programa generando el ejecutable **serialService**:  

```console
username@host:~/cese-sopg-tp1$ ./compilar.sh
```

## Ejecución

Desde la raíz, una vez compilado, abrir un terminal y ejecutar lo siguiente:  

```console
username@host:~/cese-sopg-tp1$ ./serialService
```

