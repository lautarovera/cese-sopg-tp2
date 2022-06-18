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

## Diseño

El software se basa en la utilización de dos módulos o componentes, uno llamado Serial Manager y otro llamado Interface Manager:
+ **Serial Manager**: Es el encargado de gestionar la comunicación con *Serial Emulator*, abriendo un *socket* TCP como cliente.
+ **Interface Manager**: Es el encargado de gestionar la comunicación con *Interface Service*, abriendo un *socket* TCP como servidor.
Se consideró la utilización de un hilo además del principal, ejecutándose dos tareas de manera concurrente:
+ **serial_task**: Recibe datos del servidor del *Serial Emulator*, enviando sólo tramas válidas a *Interface Service*. Se ejecuta en el hilo principal.
+ **interface_task**: Recibe datos del cliente del *Interface Service*, enviando sólo tramas válidas a *Serial Emulator*. Se ejecuta en el hilo secundario. 
El hilo principal se encarga de inicializar ambos componentes, del manejo de las señales SIGINT y SIGTERM, y de la tarea **serial_task**.
El hilo secundario se encarga de ejecutar la tarea **interface_task**.