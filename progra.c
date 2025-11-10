p.py
from datetime import datetime
from typing import Union, Optional

# 1. Classes de Mensagem (Utilizando Herança e Encapsulamento)

class Message:
    """Classe base para todos os tipos de mensagens."""
    def __init__(self, message: str, send_date: datetime):
        # Encapsulamento: atributos protegidos
        self._message = message
        self._send_date = send_date

    @property
    def message(self) -> str:
        return self._message

    @property
    def send_date(self) -> datetime:
        return self._send_date

    def get_content(self) -> dict:
        """Retorna o conteúdo básico da mensagem. (Polimorfismo: método a ser sobrescrito)"""
        return {
            "type": "base",
            "message": self._message,
            "send_date": self._send_date.isoformat()
        }

class TextMessage(Message):
    """Mensagem simples de texto."""
    def __init__(self, message: str, send_date: datetime):
        super().__init__(message, send_date)
        
    def get_content(self) -> dict:
        """Sobrescreve para garantir o tipo correto."""
        content = super().get_content()
        content["type"] = "text"
        return content

class MediaMessage(Message):
    """Classe base para mensagens com anexos (Vídeo, Foto, Arquivo). (Herança)"""
    def __init__(self, message: str, send_date: datetime, file: str, file_format: str):
        super().__init__(message, send_date)
        self._file = file
        self._file_format = file_format

    @property
    def file(self) -> str:
        return self._file

    @property
    def file_format(self) -> str:
        return self._file_format

    def get_content(self) -> dict:
        """Adiciona informações de mídia ao conteúdo."""
        content = super().get_content()
        content.update({
            "file": self._file,
            "format": self._file_format
        })
        return content

class VideoMessage(MediaMessage):
    """Mensagem de vídeo com duração. (Herança)"""
    def __init__(self, message: str, send_date: datetime, file: str, file_format: str, duration: int):
        super().__init__(message, send_date, file, file_format)
        self._duration = duration

    def get_content(self) -> dict:
        """Sobrescreve o método get_content (Polimorfismo)."""
        content = super().get_content()
        content["type"] = "video"
        content["duration"] = self._duration
        return content

class PhotoMessage(MediaMessage):
    """Mensagem de foto. (Herança)"""
    def __init__(self, message: str, send_date: datetime, file: str, file_format: str):
        super().__init__(message, send_date, file, file_format)

    def get_content(self) -> dict:
        """Sobrescreve o método get_content (Polimorfismo)."""
        content = super().get_content()
        content["type"] = "photo"
        return content

class FileMessage(MediaMessage):
    """Mensagem de arquivo genérico. (Herança)"""
    def __init__(self, message: str, send_date: datetime, file: str, file_format: str):
        super().__init__(message, send_date, file, file_format)

    def get_content(self) -> dict:
        """Sobrescreve o método get_content (Polimorfismo)."""
        content = super().get_content()
        content["type"] = "file"
        return content

# Fim das Classes de Mensagem
# # # 2. Classes de Canal (Utilizando Herança e Polimorfismo)
# 
class Channel:
    """Classe base abstrata para os canais de comunicação."""
    def __init__(self, name: str):
        self._name = name

    @property
    def name(self) -> str:
        return self._name

    def send_message(self, message: Message, recipient: str) -> bool:
        """
        Método polimórfico para enviar a mensagem.
        Deve ser implementado pelas subclasses.
        """
        raise NotImplementedError("O método 'send_message' deve ser implementado na subclasse.")

    def _log_send(self, message_content: dict, recipient: str):
        """Método auxiliar para simular o envio e logar a operação."""
        print(f"--- Enviando para {self._name} ---")
        print(f"Destinatário: {recipient}")
        print(f"Conteúdo da Mensagem: {message_content}")
        print(f"Status: SUCESSO")
        print("-" * (len(self._name) + 20))
        return True

class PhoneBasedChannel(Channel):
    """Canais que usam número de telefone como destinatário (WhatsApp, Telegram)."""
    def send_message(self, message: Message, recipient: str) -> bool:
        if not recipient.isdigit():
            print(f"ERRO: O canal {self.name} requer um número de telefone. Recebido: {recipient}")
            return False
        return self._log_send(message.get_content(), recipient)

class UsernameBasedChannel(Channel):
    """Canais que usam nome de usuário como destinatário (Facebook, Instagram, Telegram)."""
    def send_message(self, message: Message, recipient: str) -> bool:
        if not recipient.startswith('@'):
            print(f"AVISO: O canal {self.name} geralmente usa @username. Recebido: {recipient}")
        return self._log_send(message.get_content(), recipient)

class WhatsAppChannel(PhoneBasedChannel):
    def __init__(self):
        super().__init__("WhatsApp")

class FacebookChannel(UsernameBasedChannel):
    def __init__(self):
        super().__init__("Facebook")

class InstagramChannel(UsernameBasedChannel):
    def __init__(self):
        super().__init__("Instagram")

class TelegramChannel(Channel):
    """
    Canal híbrido que aceita tanto número de telefone quanto nome de usuário.
    (Exemplo de Polimorfismo mais complexo)
    """
    def __init__(self):
        super().__init__("Telegram")

    def send_message(self, message: Message, recipient: str) -> bool:
        if recipient.isdigit():
            print("INFO: Telegram detectou número de telefone.")
        elif recipient.startswith('@'):
            print("INFO: Telegram detectou nome de usuário.")
        else:
            print(f"AVISO: Telegram não conseguiu identificar o formato do destinatário. Tentando enviar para: {recipient}")
            
        return self._log_send(message.get_content(), recipient)

# 3. Classe Principal (Chatbot)
class Chatbot:
    """Gerencia o envio de mensagens para diferentes canais."""
    def __init__(self):
        self.channels = {
            "whatsapp": WhatsAppChannel(),
            "telegram": TelegramChannel(),
            "facebook": FacebookChannel(),
            "instagram": InstagramChannel(),
        }

    def send(self, channel_name: str, recipient: str, message: Message) -> bool:
        """
        Método principal para enviar a mensagem.
        
        :param channel_name: Nome do canal (ex: 'whatsapp', 'telegram').
        :param recipient: Destinatário (número de telefone ou @username).
        :param message: Objeto da mensagem (TextMessage, VideoMessage, etc.).
        :return: True se o envio for bem-sucedido, False caso contrário.
        """
        channel = self.channels.get(channel_name.lower())
        
        if not channel:
            print(f"ERRO: Canal '{channel_name}' não suportado.")
            return False
            
        # Polimorfismo em ação: O método send_message correto é chamado
        # dependendo do tipo de objeto 'channel'.
        return channel.send_message(message, recipient)


if __name__ == "__main__":
    
    # Importando datetime para simular a data de envio
    from datetime import datetime
    
    bot = Chatbot()
    
    # 1. Mensagem de Texto
    text_msg = TextMessage(
        message="Olá! Esta é uma mensagem de teste.",
        send_date=datetime.now()
           print("\n[Teste 6: WhatsApp - Destinatário Inválido]")
    bot.send("whatsapp", "@usuario_invalido", text_msg)

