#include "tCecRpi.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

tCecRpi::tCecRpi()
{
    libcec_configuration g_config;
    libcec_connection_t g_iface;
    char strPort[10];

    // Configuration de notre instance de libcec
    libcec_clear_configuration(&g_config);
    g_config.clientVersion        = LIBCEC_VERSION_CURRENT;
    g_config.bActivateSource      = 0;
    snprintf(g_config.strDeviceName, sizeof(g_config.strDeviceName), "libCEC4");
    g_config.deviceTypes.types[0] = CEC_DEVICE_TYPE_RECORDING_DEVICE;

    // Initialisation
    g_iface = libcec_initialise(&g_config);
    if (g_iface == NULL)
    {
        printf("Impossible d'initialiser libCEC\n");
        return -1;
    }

    // Appel obligatoire après l'initialisation de la bibliothèque pour initialiser la machine hôte
    libcec_init_video_standalone(g_iface);

    // Recherche du périphérique de connexion
    cec_adapter devices[10];
    int8_t iDevicesFound;
    printf("Autodétection périphérique : ");

    iDevicesFound = libcec_find_adapters(g_iface, devices, sizeof(devices) / sizeof(devices), NULL);
    if (iDevicesFound <= 0)
    {
        printf("ECHEC\n");
        libcec_destroy(g_iface);
        return 1;
    }
    else
    {
        printf("\n Chemin:     %s\n Port:       %s\n\n", devices[0].path, devices[0].comm);
        strcpy(strPort, devices[0].comm);
    }

    // Ouverture de la connexion
    printf("ouverture d'une connexion à l'adaptateur CEC...\n");
    if (!libcec_open(g_iface, strPort, 5000))
    {
        printf("impossible d'ouvrir le périphérique avec le port %s\n", strPort);
        libcec_destroy(g_iface);
        return 1;
    }


    // Nettoyage (fermeture de la bibliothèque)
    libcec_destroy(g_iface);

}
