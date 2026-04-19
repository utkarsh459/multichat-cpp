import 'dart:io';
import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

// ─── Config ───────────────────────────────────────────────
const String kHost = '208.68.37.118';
const int kPort = 8080;

void main() {
  runApp(const ChatApp());
}

// ─── App Root ─────────────────────────────────────────────
class ChatApp extends StatelessWidget {
  const ChatApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Chat App',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        useMaterial3: true,
        colorScheme: ColorScheme.fromSeed(
          seedColor: const Color(0xFF2563EB),
          brightness: Brightness.light,
        ),
        fontFamily: 'SF Pro Display',
      ),
      home: const UsernameScreen(),
    );
  }
}

// ─── Message Model ────────────────────────────────────────
class ChatMessage {
  final String raw; // full "name: msg" string
  final String sender;
  final String text;
  final bool isMe;
  final DateTime time;

  ChatMessage({
    required this.raw,
    required this.sender,
    required this.text,
    required this.isMe,
    required this.time,
  });

  static ChatMessage parse(String raw, String myName) {
    final colonIdx = raw.indexOf(': ');
    if (colonIdx == -1) {
      return ChatMessage(
        raw: raw,
        sender: 'Unknown',
        text: raw,
        isMe: false,
        time: DateTime.now(),
      );
    }
    final sender = raw.substring(0, colonIdx).trim();
    final text = raw.substring(colonIdx + 2).trim();
    return ChatMessage(
      raw: raw,
      sender: sender,
      text: text,
      isMe: sender == myName,
      time: DateTime.now(),
    );
  }
}

// ─── Username Screen ──────────────────────────────────────
class UsernameScreen extends StatefulWidget {
  const UsernameScreen({super.key});

  @override
  State<UsernameScreen> createState() => _UsernameScreenState();
}

class _UsernameScreenState extends State<UsernameScreen>
    with SingleTickerProviderStateMixin {
  final _ctrl = TextEditingController();
  late AnimationController _animCtrl;
  late Animation<double> _fadeAnim;
  late Animation<Offset> _slideAnim;
  bool _loading = false;
  String? _error;

  @override
  void initState() {
    super.initState();
    _animCtrl = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 700),
    );
    _fadeAnim = CurvedAnimation(parent: _animCtrl, curve: Curves.easeOut);
    _slideAnim = Tween<Offset>(
      begin: const Offset(0, 0.12),
      end: Offset.zero,
    ).animate(CurvedAnimation(parent: _animCtrl, curve: Curves.easeOutCubic));
    _animCtrl.forward();
  }

  @override
  void dispose() {
    _ctrl.dispose();
    _animCtrl.dispose();
    super.dispose();
  }

  Future<void> _proceed() async {
    final name = _ctrl.text.trim();
    if (name.isEmpty) {
      setState(() => _error = 'Please enter a name');
      return;
    }
    setState(() {
      _loading = true;
      _error = null;
    });

    try {
      final socket = await Socket.connect(
        kHost,
        kPort,
      ).timeout(const Duration(seconds: 5));

      if (!mounted) {
        socket.destroy();
        return;
      }

      Navigator.of(context).pushReplacement(
        MaterialPageRoute(
          builder: (_) => ChatScreen(username: name, socket: socket),
        ),
      );
    } on SocketException catch (e) {
      setState(() {
        _error = 'Cannot connect: ${e.message}';
        _loading = false;
      });
    } on TimeoutException {
      setState(() {
        _error = 'Connection timed out. Is the server running?';
        _loading = false;
      });
    } catch (e) {
      setState(() {
        _error = 'Error: $e';
        _loading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final scheme = Theme.of(context).colorScheme;
    return Scaffold(
      backgroundColor: const Color(0xFFF8FAFC),
      body: Center(
        child: FadeTransition(
          opacity: _fadeAnim,
          child: SlideTransition(
            position: _slideAnim,
            child: ConstrainedBox(
              constraints: const BoxConstraints(maxWidth: 380),
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 32),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    // Icon
                    Container(
                      width: 64,
                      height: 64,
                      decoration: BoxDecoration(
                        color: scheme.primary,
                        borderRadius: BorderRadius.circular(18),
                      ),
                      child: const Icon(
                        Icons.chat_rounded,
                        color: Colors.white,
                        size: 34,
                      ),
                    ),
                    const SizedBox(height: 28),
                    Text(
                      'Chat App',
                      style: TextStyle(
                        fontSize: 30,
                        fontWeight: FontWeight.w700,
                        color: const Color(0xFF0F172A),
                        letterSpacing: -0.5,
                      ),
                    ),
                    const SizedBox(height: 6),
                    Text(
                      'Enter your name to join the room',
                      style: TextStyle(
                        fontSize: 15,
                        color: const Color(0xFF64748B),
                      ),
                    ),
                    const SizedBox(height: 36),
                    // Name field
                    TextField(
                      controller: _ctrl,
                      autofocus: true,
                      textInputAction: TextInputAction.go,
                      onSubmitted: (_) => _proceed(),
                      style: const TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.w500,
                        color: Color(0xFF0F172A),
                      ),
                      decoration: InputDecoration(
                        hintText: 'Your name',
                        hintStyle: const TextStyle(color: Color(0xFFCBD5E1)),
                        filled: true,
                        fillColor: Colors.white,
                        errorText: _error,
                        prefixIcon: const Icon(
                          Icons.person_outline_rounded,
                          color: Color(0xFF94A3B8),
                        ),
                        contentPadding: const EdgeInsets.symmetric(
                          horizontal: 16,
                          vertical: 16,
                        ),
                        border: OutlineInputBorder(
                          borderRadius: BorderRadius.circular(14),
                          borderSide: const BorderSide(
                            color: Color(0xFFE2E8F0),
                          ),
                        ),
                        enabledBorder: OutlineInputBorder(
                          borderRadius: BorderRadius.circular(14),
                          borderSide: const BorderSide(
                            color: Color(0xFFE2E8F0),
                          ),
                        ),
                        focusedBorder: OutlineInputBorder(
                          borderRadius: BorderRadius.circular(14),
                          borderSide: BorderSide(
                            color: scheme.primary,
                            width: 1.8,
                          ),
                        ),
                      ),
                    ),
                    const SizedBox(height: 16),
                    // Join button
                    SizedBox(
                      width: double.infinity,
                      height: 50,
                      child: FilledButton(
                        onPressed: _loading ? null : _proceed,
                        style: FilledButton.styleFrom(
                          backgroundColor: scheme.primary,
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(14),
                          ),
                        ),
                        child: _loading
                            ? const SizedBox(
                                width: 20,
                                height: 20,
                                child: CircularProgressIndicator(
                                  strokeWidth: 2,
                                  color: Colors.white,
                                ),
                              )
                            : const Text(
                                'Join Chat',
                                style: TextStyle(
                                  fontSize: 15,
                                  fontWeight: FontWeight.w600,
                                ),
                              ),
                      ),
                    ),
                    const SizedBox(height: 20),
                    // Server info
                    Row(
                      children: [
                        Icon(
                          Icons.circle,
                          size: 8,
                          color: const Color(0xFF22C55E),
                        ),
                        const SizedBox(width: 6),
                        Text(
                          '$kHost:$kPort',
                          style: const TextStyle(
                            fontSize: 12,
                            color: Color(0xFF94A3B8),
                            fontFamily: 'monospace',
                          ),
                        ),
                      ],
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

// ─── Chat Screen ──────────────────────────────────────────
class ChatScreen extends StatefulWidget {
  final String username;
  final Socket socket;

  const ChatScreen({super.key, required this.username, required this.socket});

  @override
  State<ChatScreen> createState() => _ChatScreenState();
}

class _ChatScreenState extends State<ChatScreen> {
  final _msgCtrl = TextEditingController();
  final _scrollCtrl = ScrollController();
  final List<ChatMessage> _messages = [];
  late StreamSubscription _sub;
  bool _connected = true;
  String _inputBuffer = ''; // handles partial TCP reads

  @override
  void initState() {
    super.initState();
    _listenToSocket();
  }

  // ── Socket listener ─────────────────────────────────────
  void _listenToSocket() {
    _sub = widget.socket
        .cast<List<int>>()
        .transform(utf8.decoder)
        .listen(
          (data) {
            // TCP can split or merge frames — split on newlines / handle partial
            _inputBuffer += data;
            final lines = _inputBuffer.split('\n');
            _inputBuffer = lines.last; // incomplete tail

            for (int i = 0; i < lines.length - 1; i++) {
              final line = lines[i].trim();
              if (line.isEmpty) continue;
              _addMessage(line);
            }

            // Also handle servers that don't use newlines (your current C++ server)
            // If buffer has no newline but has content, treat as a full message
            if (_inputBuffer.isNotEmpty &&
                !_inputBuffer.contains('\n') &&
                data.endsWith(' ') == false) {
              // Flush after short delay — handles no-newline servers
              Future.delayed(const Duration(milliseconds: 20), () {
                if (_inputBuffer.isNotEmpty) {
                  _addMessage(_inputBuffer.trim());
                  _inputBuffer = '';
                }
              });
            }
          },
          onDone: _onDisconnect,
          onError: (_) => _onDisconnect(),
        );
  }

  void _addMessage(String raw) {
    if (raw.isEmpty) return;
    final msg = ChatMessage.parse(raw, widget.username);
    setState(() => _messages.add(msg));
    _scrollToBottom();
  }

  void _onDisconnect() {
    if (!mounted) return;
    setState(() => _connected = false);
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: const Text('Disconnected from server'),
        backgroundColor: const Color(0xFFEF4444),
        behavior: SnackBarBehavior.floating,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
      ),
    );
  }

  // ── Send message ─────────────────────────────────────────
  void _sendMessage() {
    final text = _msgCtrl.text.trim();
    if (text.isEmpty || !_connected) return;

    final full = '${widget.username}: $text';
    try {
      widget.socket.write(full);
    } catch (_) {
      _onDisconnect();
      return;
    }
    _msgCtrl.clear();
  }

  void _scrollToBottom() {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (_scrollCtrl.hasClients) {
        _scrollCtrl.animateTo(
          _scrollCtrl.position.maxScrollExtent,
          duration: const Duration(milliseconds: 280),
          curve: Curves.easeOutCubic,
        );
      }
    });
  }

  @override
  void dispose() {
    _sub.cancel();
    widget.socket.destroy();
    _msgCtrl.dispose();
    _scrollCtrl.dispose();
    super.dispose();
  }

  // ── Build ─────────────────────────────────────────────────
  @override
  Widget build(BuildContext context) {
    final scheme = Theme.of(context).colorScheme;
    return Scaffold(
      backgroundColor: const Color(0xFFF1F5F9),
      appBar: _buildAppBar(scheme),
      body: Column(
        children: [
          Expanded(child: _buildMessageList()),
          _buildInputBar(scheme),
        ],
      ),
    );
  }

  PreferredSizeWidget _buildAppBar(ColorScheme scheme) {
    return AppBar(
      backgroundColor: Colors.white,
      elevation: 0,
      surfaceTintColor: Colors.transparent,
      systemOverlayStyle: SystemUiOverlayStyle.dark,
      titleSpacing: 0,
      leading: IconButton(
        icon: const Icon(Icons.arrow_back_ios_new_rounded, size: 18),
        onPressed: () {
          widget.socket.destroy();
          Navigator.of(context).pushReplacement(
            MaterialPageRoute(builder: (_) => const UsernameScreen()),
          );
        },
      ),
      title: Row(
        children: [
          CircleAvatar(
            radius: 18,
            backgroundColor: scheme.primary.withOpacity(0.12),
            child: Text(
              widget.username[0].toUpperCase(),
              style: TextStyle(
                color: scheme.primary,
                fontWeight: FontWeight.w700,
                fontSize: 15,
              ),
            ),
          ),
          const SizedBox(width: 10),
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text(
                'Chat Room',
                style: TextStyle(
                  fontSize: 15,
                  fontWeight: FontWeight.w700,
                  color: Color(0xFF0F172A),
                ),
              ),
              Row(
                children: [
                  Icon(
                    Icons.circle,
                    size: 7,
                    color: _connected
                        ? const Color(0xFF22C55E)
                        : const Color(0xFFEF4444),
                  ),
                  const SizedBox(width: 4),
                  Text(
                    _connected ? 'Connected' : 'Disconnected',
                    style: TextStyle(
                      fontSize: 11,
                      color: _connected
                          ? const Color(0xFF22C55E)
                          : const Color(0xFFEF4444),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ],
      ),
      bottom: PreferredSize(
        preferredSize: const Size.fromHeight(1),
        child: Container(color: const Color(0xFFE2E8F0), height: 1),
      ),
    );
  }

  Widget _buildMessageList() {
    if (_messages.isEmpty) {
      return Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(
              Icons.chat_bubble_outline_rounded,
              size: 48,
              color: const Color(0xFFCBD5E1),
            ),
            const SizedBox(height: 12),
            const Text(
              'No messages yet',
              style: TextStyle(color: Color(0xFF94A3B8), fontSize: 14),
            ),
            const SizedBox(height: 4),
            const Text(
              'Say hello!',
              style: TextStyle(color: Color(0xFFCBD5E1), fontSize: 12),
            ),
          ],
        ),
      );
    }

    return ListView.builder(
      controller: _scrollCtrl,
      padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 12),
      itemCount: _messages.length,
      itemBuilder: (ctx, i) {
        final msg = _messages[i];
        final prev = i > 0 ? _messages[i - 1] : null;
        final showSender = prev == null || prev.sender != msg.sender;
        return _MessageBubble(
          message: msg,
          showSenderName: showSender && !msg.isMe,
        );
      },
    );
  }

  Widget _buildInputBar(ColorScheme scheme) {
    return Container(
      color: Colors.white,
      padding: EdgeInsets.only(
        left: 16,
        right: 12,
        top: 10,
        bottom: MediaQuery.of(context).padding.bottom + 10,
      ),
      child: Row(
        children: [
          Expanded(
            child: TextField(
              controller: _msgCtrl,
              enabled: _connected,
              textInputAction: TextInputAction.send,
              onSubmitted: (_) => _sendMessage(),
              style: const TextStyle(fontSize: 15, color: Color(0xFF0F172A)),
              decoration: InputDecoration(
                hintText: _connected ? 'Type a message...' : 'Disconnected',
                hintStyle: const TextStyle(
                  color: Color(0xFFCBD5E1),
                  fontSize: 15,
                ),
                filled: true,
                fillColor: const Color(0xFFF8FAFC),
                contentPadding: const EdgeInsets.symmetric(
                  horizontal: 16,
                  vertical: 12,
                ),
                border: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(24),
                  borderSide: const BorderSide(color: Color(0xFFE2E8F0)),
                ),
                enabledBorder: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(24),
                  borderSide: const BorderSide(color: Color(0xFFE2E8F0)),
                ),
                focusedBorder: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(24),
                  borderSide: BorderSide(color: scheme.primary, width: 1.5),
                ),
                disabledBorder: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(24),
                  borderSide: const BorderSide(color: Color(0xFFE2E8F0)),
                ),
              ),
            ),
          ),
          const SizedBox(width: 8),
          // Send button
          AnimatedContainer(
            duration: const Duration(milliseconds: 200),
            child: Material(
              color: _connected ? scheme.primary : const Color(0xFFCBD5E1),
              borderRadius: BorderRadius.circular(50),
              child: InkWell(
                borderRadius: BorderRadius.circular(50),
                onTap: _connected ? _sendMessage : null,
                child: const Padding(
                  padding: EdgeInsets.all(12),
                  child: Icon(
                    Icons.send_rounded,
                    color: Colors.white,
                    size: 20,
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

// ─── Message Bubble ───────────────────────────────────────
class _MessageBubble extends StatelessWidget {
  final ChatMessage message;
  final bool showSenderName;

  const _MessageBubble({required this.message, required this.showSenderName});

  @override
  Widget build(BuildContext context) {
    final scheme = Theme.of(context).colorScheme;
    final isMe = message.isMe;

    return Padding(
      padding: const EdgeInsets.only(bottom: 4),
      child: Column(
        crossAxisAlignment: isMe
            ? CrossAxisAlignment.end
            : CrossAxisAlignment.start,
        children: [
          if (showSenderName)
            Padding(
              padding: const EdgeInsets.only(left: 4, bottom: 3, top: 8),
              child: Text(
                message.sender,
                style: const TextStyle(
                  fontSize: 11,
                  fontWeight: FontWeight.w600,
                  color: Color(0xFF64748B),
                ),
              ),
            ),
          Row(
            mainAxisAlignment: isMe
                ? MainAxisAlignment.end
                : MainAxisAlignment.start,
            children: [
              ConstrainedBox(
                constraints: BoxConstraints(
                  maxWidth: MediaQuery.of(context).size.width * 0.72,
                ),
                child: Container(
                  padding: const EdgeInsets.symmetric(
                    horizontal: 14,
                    vertical: 10,
                  ),
                  decoration: BoxDecoration(
                    color: isMe ? scheme.primary : Colors.white,
                    borderRadius: BorderRadius.only(
                      topLeft: const Radius.circular(18),
                      topRight: const Radius.circular(18),
                      bottomLeft: Radius.circular(isMe ? 18 : 4),
                      bottomRight: Radius.circular(isMe ? 4 : 18),
                    ),
                    boxShadow: [
                      BoxShadow(
                        color: Colors.black.withOpacity(0.05),
                        blurRadius: 4,
                        offset: const Offset(0, 1),
                      ),
                    ],
                  ),
                  child: Text(
                    message.text,
                    style: TextStyle(
                      fontSize: 15,
                      color: isMe ? Colors.white : const Color(0xFF0F172A),
                      height: 1.35,
                    ),
                  ),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
