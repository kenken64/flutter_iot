import 'dart:async';
import 'dart:io' show Platform;

import 'package:carousel_slider/carousel_slider.dart';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';


final List<String> imgList = [
  'https://i.ytimg.com/vi/ifrHogDujXw/maxresdefault.jpg',
  'https://compote.slate.com/images/eb1853bf-8feb-471a-8402-00bd5bf8b30f.jpg',
  'https://www.simpsondoor.com/door-idea-gallery/fullsize/wood-double-doors-8420.jpg',
  'https://d2gk6qz8djobw9.cloudfront.net/slider/1499945767.jpg',
];

final List<String> imgIconList = [
  'assets/door-closed.png',
  'assets/door-open.png',
  'assets/humidity.png',
  'assets/temperature.png',
  'assets/motion.png',
];

final List<String> imgObjectNames = [
  'Temperature',
  'Humidity',
  'Door Sensor',
  'Motion',
];


Future<void> main() async {
  final FirebaseApp app = await FirebaseApp.configure(
    name: 'mijia-thermostat',
    options: Platform.isIOS
        ? const FirebaseOptions(
            googleAppID: '1:999431981277:ios:8f973cd62f1f2d86',
            gcmSenderID: '999431981277',
            databaseURL: 'https://mijia-thermostat.firebaseio.com',
          )
        : const FirebaseOptions(
            googleAppID: '1:999431981277:android:70678bb0b07b2375',
            apiKey: 'AIzaSyCfGnx3sDUolTv7-4t7-jVI19UhIYb6AYU',
            databaseURL: 'https://mijia-thermostat.firebaseio.com',
          ),
  );
  runApp(MaterialApp(
    title: 'Flutter iot',
    home: CarouselDemo(app: app),
  ));
}
//void main() => runApp(CarouselDemo());

final Widget placeholder = Container(color: Colors.grey);

final List child = map<Widget>(
  imgList,
  (index, i) {
    return Container(
      margin: EdgeInsets.all(5.0),
      child: ClipRRect(
        borderRadius: BorderRadius.all(Radius.circular(5.0)),
        child: Stack(children: <Widget>[
          Image.network(i, fit: BoxFit.cover, width: 500.0,
                  height: 400.0,),
          Positioned(
            bottom: 0.0,
            left: 0.0,
            right: 0.0,
            child: Container(
              decoration: BoxDecoration(
                gradient: LinearGradient(
                  colors: [Color.fromARGB(200, 0, 0, 0), Color.fromARGB(0, 0, 0, 0)],
                  begin: Alignment.bottomCenter,
                  end: Alignment.topCenter,
                ),
              ),
              padding: EdgeInsets.symmetric(vertical: 10.0, horizontal: 20.0),
              child: Text(
                imgObjectNames[index],
                style: TextStyle(
                  color: Colors.white,
                  fontSize: 20.0,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),
          ),
        ]),
      ),
    );
  },
).toList();

List<T> map<T>(List list, Function handler) {
  List<T> result = [];
  for (var i = 0; i < list.length; i++) {
    result.add(handler(i, list[i]));
  }

  return result;
}


List iconsIot = [
  Icon(
    Icons.ac_unit,
    color: Colors.blue[500],
    size: 50,
  ),
  Icon(
    Icons.beach_access,
    color: Colors.blue[500],
    size: 50,
  ),
  Icon(
    Icons.beenhere,
    color: Colors.blue[500],
    size: 50,
  ),
  Icon(
    Icons.directions_run,
    color: Colors.blue[500],
    size: 50,
  ),
];

class CarouselWithIndicator extends StatefulWidget {
  CarouselWithIndicator({this.app});
  final FirebaseApp app;
  @override
  _CarouselWithIndicatorState createState() => _CarouselWithIndicatorState();
}

class _CarouselWithIndicatorState extends State<CarouselWithIndicator> {
  int _current = 0;
  var iconIndex = 0;
  var descriptions = "";
  var value = "";
  var values;
  bool visibilityTag = false;
  DatabaseReference _tempThresholdRef;
  DatabaseReference _notificationRef;
  @override
  void dispose() {
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    _fetchData(_current);
     _tempThresholdRef = FirebaseDatabase.instance.reference().child('tempthreshold');
    // Demonstrates configuring the database directly
    final FirebaseDatabase database = FirebaseDatabase(app: widget.app);
    _notificationRef = database.reference().child('notification');
    database.reference().child('tempthreshold').once().then((DataSnapshot snapshot) {
      print('Connected to second database and read ${snapshot.value}');
    });
    database.setPersistenceEnabled(true);
    database.setPersistenceCacheSizeBytes(10000000);
  }

  _createRecord(){
    _tempThresholdRef.set({
      'value': '4',
      'message': 'temperature is above 4 Celcius'
    });
  }

  _fetchData(_current) async {
    print("Attempting to fetch data from network..");
    if(_current == 0){
      final url = "http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/get/V0";
      final response = await http.get(url);
      if(response.statusCode == 200){
        print(response.body);
        final map = json.decode(response.body);
        setState(() {
          this.descriptions = "Temperature";
          var withoutbracket = map.toString().replaceAll("[", '');
          withoutbracket = withoutbracket.replaceAll("]", '');
          this.value = withoutbracket;
          this.iconIndex = _current;
          this.visibilityTag = true;
        });
      }
    }
    
    if(_current == 1){
      final url2 = "http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/get/V1";
      final response2 = await http.get(url2);
      if(response2.statusCode == 200){
        print(response2.body);
        final map = json.decode(response2.body);
        setState(() {
          this.descriptions = "Humidity";
          var withoutbracket = map.toString().replaceAll("[", '');
          withoutbracket = withoutbracket.replaceAll("]", '');
          this.value = withoutbracket;
          this.iconIndex = _current;
        });
      }
    }
    
    if(_current == 2){
      print("door sensor >>");
      final url3 = "http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/get/V2";
      final response3 = await http.get(url3);
      if(response3.statusCode == 200){
        print(response3.body);
        final map = json.decode(response3.body);
        setState(() {
          this.descriptions = "Door state";
          var withoutbracket = map.toString().replaceAll("[", '');
          withoutbracket = withoutbracket.replaceAll("]", '');
          this.value = withoutbracket;
          this.iconIndex = _current;
        });
      }
    }

    if(_current == 3){
      print("motion >>");
      final url3 = "http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/get/V3";
      final response3 = await http.get(url3);
      if(response3.statusCode == 200){
        print(response3.body);
        final map = json.decode(response3.body);
        setState(() {
          this.descriptions = "Movement";
          var withoutbracket = map.toString().replaceAll("[", '');
          withoutbracket = withoutbracket.replaceAll("]", '');
          if(withoutbracket == '1'){
            withoutbracket = "Motion detected";
          }else{
            withoutbracket = "No motion detected";
          }
          this.value = withoutbracket;
          this.iconIndex = _current;
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Column(children: [
      CarouselSlider(
        items: child,
        autoPlay: false,
        enlargeCenterPage: true,
        aspectRatio: 2.0,
        onPageChanged: (index) {
          setState(() {
            _current = index;
            _fetchData(_current);
            _createRecord();
          });
        },
      ),
      Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: map<Widget>(
          imgList,
          (index, url) {
            return Container(
              width: 8.0,
              height: 8.0,
              margin: EdgeInsets.symmetric(vertical: 10.0, horizontal: 2.0),
              decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: _current == index
                      ? Color.fromRGBO(0, 0, 0, 0.9)
                      : Color.fromRGBO(0, 0, 0, 0.4)),
            );
          },
        ),
      ),
      SizedBox(
        height: 210,
        child: Card(
          child: Column(
            children: [
              ListTile(
                title: Text(this.descriptions,
                    style: TextStyle(fontSize:30,fontWeight: FontWeight.w500)),
                subtitle: Text(this.value ,
                    style: TextStyle(fontSize:21)),
                leading: iconsIot[this.iconIndex],
              ),
              Divider(),
              visibilityTag ? Text("1"): Text("2")
            ],
          ),
        ),
      ),
    ]);
  }
}

class CarouselDemo extends StatelessWidget {

  CarouselDemo({this.app});
  final FirebaseApp app;
  
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Internet of Things Demo',
      home: Scaffold(
        appBar: AppBar(title: Text('Flutter Internet of Things Demo')),
        body: ListView(
          children: <Widget>[
            Padding(
                padding: EdgeInsets.symmetric(vertical: 15.0),
                child: Column(children: [
                  Text('Sensor data'),
                  CarouselWithIndicator(app: app),
            ])),
          ],
        ),
      ),
    );
  }
}