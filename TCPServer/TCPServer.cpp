#include <winsock2.h>
#include <stdio.h>
#include <stdint.h>
#include<winsock2.h>
#include<Ws2tcpip.h>

#pragma comment( lib, "Ws2_32.lib" ) 

int main(int argc, char** argv)
{
   WSADATA            wsaData;
   SOCKET             ReceivingSocket;
   SOCKADDR_IN        ReceiverAddr;
   int                Port = 7898;
   char          ReceiveBuf[1024];
   int                BufLength = 1024;
   SOCKADDR_IN        SenderAddr;
   int                SenderAddrSize = sizeof(SenderAddr);
   int                ByteReceived = 5;

   if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
   {
      printf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
      return -1;
   }
   else
      printf("Server: The Winsock DLL status is %s.\n", wsaData.szSystemStatus);

   ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if(ReceivingSocket == INVALID_SOCKET)
   {
      printf("Server: Error at socket(): %ld\n", WSAGetLastError());
      WSACleanup();
      return -1;
   }
   else
      printf("Server: socket() is OK!\n");

   ReceiverAddr.sin_family = AF_INET;
   ReceiverAddr.sin_port = htons(Port);
   ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   if(bind(ReceivingSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
   {
      printf("Server: bind() failed! Error: %ld.\n", WSAGetLastError());
      closesocket(ReceivingSocket);
      WSACleanup();
      return -1;
   }
   else
      printf("Server: bind() is OK!\n");

   getsockname(ReceivingSocket, (SOCKADDR*)&ReceiverAddr, (int*)sizeof(ReceiverAddr));

   char l_pszBuf[50] = {0};
   inet_ntop(AF_INET, (&ReceiverAddr.sin_addr), l_pszBuf, sizeof(l_pszBuf));

   printf("Server: Receiving IP(s) used: %s\n", l_pszBuf);
   printf("Server: Receiving port used: %d\n", htons(ReceiverAddr.sin_port));

   printf("Server: I\'m ready to receive a datagram...\n");

   ByteReceived = recvfrom(ReceivingSocket, ReceiveBuf, BufLength, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
   while(ByteReceived)
   {
      printf("Server: Total Bytes received: %d\n", ByteReceived);
      
      printf("Server: The data ", ReceiveBuf);
      for(size_t i = 0; i < ByteReceived; ++i)
      {
         uint8_t l_u8Byte = ReceiveBuf[i];
         if(i == 0)
            printf(l_u8Byte > 0xF ? "0x%x" : "0x0%x", (uint8_t)ReceiveBuf[i]);
         else
            printf(l_u8Byte > 0xF ? ", 0x%x" : ", 0x0%x", (uint8_t)ReceiveBuf[i]);
      }
      printf("\n", ReceiveBuf);

      getpeername(ReceivingSocket, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

      char l_pszBuf2[50] = {0};
      inet_ntop(AF_INET, (&ReceiverAddr.sin_addr), l_pszBuf2, sizeof(l_pszBuf2));

      printf("Server: Sending IP used: %s\n", l_pszBuf2);
      printf("Server: Sending port used: %d\n", htons(SenderAddr.sin_port));

      ByteReceived = recvfrom(ReceivingSocket, ReceiveBuf, BufLength, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
   }

   printf("Server: Connection closed with error code: %ld\n", WSAGetLastError());
   printf("Server: recvfrom() failed with error code: %d\n", WSAGetLastError());

   printf("Server: Finished receiving. Closing the listening socket...\n");
   if(closesocket(ReceivingSocket) != 0)
      printf("Server: closesocket() failed! Error code: %ld\n", WSAGetLastError());
   else
      printf("Server: closesocket() is OK\n");

   printf("Server: Cleaning up...\n");
   if(WSACleanup() != 0)
      printf("Server: WSACleanup() failed! Error code: %ld\n", WSAGetLastError());
   else
      printf("Server: WSACleanup() is OK\n");
   return 0;
}