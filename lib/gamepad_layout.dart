import 'package:flutter/material.dart';
import 'package:win_gamepad/gamepad.dart';
import 'package:win_gamepad/win_gamepad.dart';

class GamepadLayout extends StatefulWidget {
  const GamepadLayout(
      {Key? key,
      required this.gamepadStateStream,
      this.heigth = 400,
      this.width = 600})
      : super(key: key);
  final Stream<dynamic> gamepadStateStream;
  final double width;
  final double heigth;
  @override
  _GamepadLayoutState createState() => _GamepadLayoutState();
}

class _GamepadLayoutState extends State<GamepadLayout> {
  Gamepad gamepad = Gamepad();
  double width = 600;
  double height = 400;

  @override
  void initState() {
    super.initState();
    width = widget.width;
    height = widget.heigth;
    init();
  }

  void init() async {
    WinGamepad.eventStream.listen((event) {
      gamepad.update(Map<String, dynamic>.from(event));
      setState(() {
        print(gamepad.state);
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: width,
      height: height,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              Column(
                children: [
                  RoundedTrigger(
                    title: '  LT  ',
                    pressure: gamepad.state.leftTrigger,
                  ),
                  RoundedButton(
                    title: '  LB  ',
                    isPressed:
                        gamepad.state.isPressed(GamepadButton.leftShoulder),
                  ),
                ],
              ),
              Column(
                children: [
                  RoundedTrigger(
                    title: '  RT  ',
                    pressure: gamepad.state.rightTrigger,
                  ),
                  RoundedButton(
                    title: '  RB  ',
                    isPressed:
                        gamepad.state.isPressed(GamepadButton.rightShoulder),
                  ),
                ],
              ),
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Thumb(
                title: 'L',
                thumbX: gamepad.state.leftThumbX,
                thumbY: gamepad.state.leftThumbY,
                ispressed: gamepad.state.isPressed(GamepadButton.leftThumb),
              ),
              Row(
                children: [
                  RoundedButton(
                    title: 'Back',
                    isPressed: gamepad.state.isPressed(GamepadButton.back),
                  ),
                  RoundedButton(
                    title: 'Start',
                    isPressed: gamepad.state.isPressed(GamepadButton.start),
                  ),
                ],
              ),
              FaceButtons(gamepad: gamepad),
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Dpad(gamepad: gamepad),
              Thumb(
                title: 'R',
                thumbX: gamepad.state.rightThumbX,
                thumbY: gamepad.state.rightThumbY,
                ispressed: gamepad.state.isPressed(GamepadButton.rightThumb),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class Thumb extends StatelessWidget {
  const Thumb({
    Key? key,
    required this.thumbX,
    required this.thumbY,
    required this.ispressed,
    required this.title,
  }) : super(key: key);

  final int thumbX;
  final int thumbY;
  final bool ispressed;
  final String title;

  @override
  Widget build(BuildContext context) {
    return Stack(
      alignment: AlignmentDirectional.center,
      children: [
        Padding(
          padding: const EdgeInsets.all(20.0),
          child: CircleAvatar(
            backgroundColor: Colors.green.shade400,
            radius: 60,
          ),
        ),
        Positioned(
          top: -(thumbY / 32768) * 40 + 50,
          left: (thumbX / 32768) * 40 + 50,
          child: FaceButton(
            title: title,
            padding: const EdgeInsets.all(0),
            color: Colors.grey.shade700,
            focusColor: Colors.grey.shade900,
            isPressed: ispressed,
            size: 30,
          ),
        ),
      ],
    );
  }
}

class RoundedTrigger extends StatelessWidget {
  const RoundedTrigger({
    Key? key,
    required this.pressure,
    required this.title,
  }) : super(key: key);

  final String title;
  final int pressure;

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.all(5),
      padding: const EdgeInsets.all(5),
      decoration: BoxDecoration(
        color: Color.lerp(
            Colors.red.shade200, Colors.red.shade900, pressure / 255),
        borderRadius: BorderRadius.circular(30),
      ),
      child: Text(
        title,
        style: const TextStyle(color: Colors.white),
      ),
    );
  }
}

class RoundedButton extends StatelessWidget {
  const RoundedButton({
    Key? key,
    required this.isPressed,
    required this.title,
  }) : super(key: key);

  final String title;
  final bool isPressed;

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.symmetric(horizontal: 40, vertical: 5),
      padding: const EdgeInsets.all(5),
      decoration: BoxDecoration(
        color: isPressed ? Colors.blueGrey.shade600 : Colors.blueGrey.shade300,
        borderRadius: BorderRadius.circular(30),
      ),
      child: Text(
        title,
        style: const TextStyle(color: Colors.white),
      ),
    );
  }
}

class Dpad extends StatelessWidget {
  const Dpad({
    Key? key,
    required this.gamepad,
  }) : super(key: key);

  final Gamepad gamepad;

  @override
  Widget build(BuildContext context) {
    return CircleAvatar(
      radius: 45,
      child: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 5),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(
              '˄',
              style: TextStyle(
                  fontSize: 22,
                  fontWeight: FontWeight.bold,
                  color: gamepad.state.isPressed(GamepadButton.dpadUp)
                      ? Colors.white
                      : Colors.white70),
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Text(
                  '<',
                  style: TextStyle(
                      fontSize: 22,
                      fontWeight: FontWeight.bold,
                      color: gamepad.state.isPressed(GamepadButton.dpadLeft)
                          ? Colors.white
                          : Colors.white70),
                ),
                Text(
                  '>',
                  style: TextStyle(
                      fontSize: 22,
                      fontWeight: FontWeight.bold,
                      color: gamepad.state.isPressed(GamepadButton.dpadRight)
                          ? Colors.white
                          : Colors.white70),
                ),
              ],
            ),
            Text(
              '˅',
              style: TextStyle(
                  fontSize: 22,
                  fontWeight: FontWeight.bold,
                  color: gamepad.state.isPressed(GamepadButton.dpadDown)
                      ? Colors.white
                      : Colors.white70),
            ),
          ],
        ),
      ),
      backgroundColor: Colors.grey.shade600,
    );
  }
}

class FaceButtons extends StatelessWidget {
  const FaceButtons({
    Key? key,
    required this.gamepad,
  }) : super(key: key);

  final Gamepad gamepad;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 150,
      height: 150,
      child: Stack(
        fit: StackFit.expand,
        children: [
          Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              FaceButton(
                title: "Y",
                isPressed: gamepad.state.isPressed(GamepadButton.y),
                color: Colors.yellow.shade300,
                focusColor: Colors.yellow.shade600,
              ),
              FaceButton(
                title: "A",
                isPressed: gamepad.state.isPressed(GamepadButton.a),
                color: Colors.green.shade300,
                focusColor: Colors.green.shade600,
              ),
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              FaceButton(
                title: "X",
                isPressed: gamepad.state.isPressed(GamepadButton.x),
                color: Colors.blue.shade300,
                focusColor: Colors.blue.shade600,
              ),
              FaceButton(
                title: "B",
                isPressed: gamepad.state.isPressed(GamepadButton.b),
                color: Colors.red.shade300,
                focusColor: Colors.red.shade600,
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class FaceButton extends StatelessWidget {
  const FaceButton({
    Key? key,
    required this.title,
    required this.color,
    required this.focusColor,
    required this.isPressed,
    this.size = 20,
    this.padding = const EdgeInsets.all(12.0),
  }) : super(key: key);
  final String title;
  final Color color;
  final Color focusColor;
  final bool isPressed;
  final double size;
  final EdgeInsets padding;
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: padding,
      child: CircleAvatar(
        radius: size,
        child: Text(
          title,
          style: const TextStyle(color: Colors.white),
        ),
        backgroundColor: isPressed ? focusColor : color,
      ),
    );
  }
}
