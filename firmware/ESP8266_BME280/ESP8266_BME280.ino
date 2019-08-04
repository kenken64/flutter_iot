#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;

/*Put your SSID & Password*/
const char* ssid = "kenken64";  // Enter SSID here
const char* password = "7730112910100";  //Enter Password here

ESP8266WebServer server(80);              
 
void setup() {
  Serial.begin(115200);
  delay(200);
  
  bme.begin(0x76);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature,float humidity,float pressure,float altitude){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>Fridge Sensor</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<meta http-equiv='refresh' content='2' >";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin: 0px;background-size: auto 100%;background-image: url('https://www.solidbackgrounds.com/images/2560x1440/2560x1440-white-solid-color-background.jpg');} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  ptr +=".temperature .reading{color: #F29C1F;}";
  ptr +=".humidity .reading{color: #3B97D3;}";
  ptr +=".pressure .reading{color: #26B99A;}";
  ptr +=".altitude .reading{color: #955BA5;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:65px}";
  ptr +="</style>";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<br>";
  ptr +="<img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAYQAAABfCAYAAADoM7CLAAAACXBIWXMAACxKAAAsSgF3enRNAAATjElEQVR4nO2dTXajOBeG36pTc2cH4VtB3ENGoSZMy72CuFZQ7hUAK+jUCgqvoJwpkyYjhu2soPEO4hXkG+gSE8fGukJC2LnPOTndlQgkhNCrn3t1P728vOBSCLP4CsACwAzADf16A2AFIK2S4tlX2QRBEMbOp0sRBBKDEjsh2GcLYFolRT1UmQRBEM6JL74LYJEVjosBAEwA5ACiIQojCLagwc7UdzmEwVnrrGpYbB/rixCEMIunAG41kt6GWTytkmLtukyCYEqYxTOoZc8pugc5wmXzFWrV4xRTAP/YyO8iBAG8UX8EQARBGBVhFgcAUighmHgtjPBhuRRBuHKUVhCc0jKESHyXRRAuRRAE4eygpc4csiwkjITPvgsgCB8R2icoIWIgjAgRBEEYmDCL5wB+Q/YKhJEhgiAIA0LLRPe+yyEIhxBBEISBoA3kFWRmIIwUEQRBGI4UwLXvQgjCMUQQBGEAwiyOAPzwXQ5B6EIEQRCGYeG7AIJwChEEQXAMeSF/810OQTiFCIIguGfmuwCCoIMIgiC4RwRBOAtEEATBPTon8QqCd0QQBMEhZF0kCGeBCIIgCIIAQARBEFwT+C6AIOgigiAIbgl8F0AQdBFBEARBEACIIAiCIAiECIIgCIIAQARBEARBIHrHVKaAH8cC1z9XSbHum8clQWfiT4/8+Wzr60g7GOR5TtRpXSVF7boMgnAJsASBPrwZgAjqAzwZDzbMYgDYAFhDxZAtbXQSex0Q52iAWZjF5ZG/raukeO5Trn2onO066wyOQvX1hF1drWyW50iec6gyNvW5BpCfek8UF3gO9WwHnyvM4i3Us+S2noUOi5vRzyjr1CEPUO8HUM+uc2jeBvTsUG383XttiWoEVa++Yz1voIIJraFEvTyUiNpCAFXuCP69wrdQ5S5xvK4D7Mo8hrp+5dPLy8vJRPQAKYA7S/k2L/tkp3OgLDOoEISuAo0sASz6CgN1sgv0f9lbqOfNbY90qRMocbyMP6ukeHdsc4938ARgZvocVKdz9P/oNwByAPe2BwD7hFmcAkgs3OoBwHy/vPRtrnD4HT4BSE0EkAYyC9j75nXYQr0Xdr/Q0Bq02vj2OGyg6jrnXthR11+PCeHe9RGAf7j5HuBrpyBQ5d7DbaN4hGoA+amEFh/8FI9VUkQmFzoUrEYYrHViYRavcfqj+d68G0vtYQsg4DwDvfccjuq0SorU8n1fsSQIne2R3ssab+vnryopesduprq/h/vONYNlgXbYbvbJbLQhEoYcu7oeXBCObipT4dZwP0K4BfArzOKS8uxiqCAjtxpleUOYxVdhFq8A/IabBjiB6lh06ukkJFw6H3lK6ZvZRN/2MGnuqUOYxfdQjd1ZnYZZvLZRpw7pbPfUiab0zy2AP2yIAd27hFraWNq43wGeAPyvSorU9mytSoqySooASmxc0NR1auNmNCuKoOrECwcFgT6OEsPGf70F8C8tCxxjyCAjkW5CmraXGKZ8N1CiMO95n0gz3TW1h2PLEk7yJoFdY5iwk02djlEUNjrLJ60Z9sz2Rn6VFM9VUsxhXxSWVVJMXW/6U4f9FaoDt8UGwNRFXcOjKLwThJYYdG7UOeSeOljfHLOcekNruj7keuUEalY173EPTud3D7ubdZ11pbG34YIJxikKNSPtHzpLDKZYFoUl3W8QWjMdG6KwRY+9sFOQKMzAe/dWODRDyOFPDEB5z4/87XHAcpSnErQ6Lp/iOUQHZtty49Tox+ZshMNYRUGLgUyWF1Cj4z4MKgYNrSWZvsxd13WVFF7Mpd8IAo04x2ACFRz5vZV1UQ2eNEdaQ2y2dTEBsCJhOieOfky0CevTdHACNSgSDkCj13mPWzxhuL3Ad1BH/lePWyzP3Gy5k/0ZQuqjEAeoD/2SXsR32F0L3OcRemvcEYY1yTvGNcbz3nQ5KOy0VGjDRLMvNyRMwgFosGS6dPTOdHZoaMPdZLVhi/P71li8OqbRNJm7ifwINZoq96c31GEG2DmMcO5dH/sDbZ7lrUhUV1AvSXek3jVC4UzTcs10h2gc9ZqRcgBNR78j/Aiz+N6TR+4W6jkC6L3j7x3T7dywDA9Qy0yvDkwkLm1HK27bTsIstu774QP6ViKo+mhmkzX9rAyXP1LwB0RLk7xajogBdntfz6BvyHDEvgDwL/Mao/bQ8o2Y4u3eXdMHrHyLZEPbU3nGuG4LpfRHX0RrySUH3jhfzNC95r7R8UloL+nQvXU701XfjTdaWjOxwHqEcl45mD81/DlUPXH3JVL0m8pz2FB+7xpyq/OJ8NaLWOfZuUtFD1BOhPX+H+h3NZRQLMIsXlCZOfWaYrg6tc4Jn5imrpMwix+h6lG7s66SoqbrOO8sZaTV8Xv5Ruk2UOXXFoYqKdZhFi877n0I1pI1lT/FcUu5pu5+hVn8E+r78CoMbUGIGNexKh94XbubUyUtcLjTe8J5fIBzg2u+nxI66sRS8mfIwZsx3IVZ3NvDWoNOT27q8EuD+6bM9CxnoCop7unIkhL6ojALs/jK90dqAgng35rJG5Pvk210jxz6grDkjK6Z1o7XAH6HWczdrM6hLwhPzPJzLeV+AIjCLI58trf2HoLuxqTWCP4YZNOcVklxBeBPKKeRDMCfZJM8hKWEMYYjWdaH1rKG4O6VcGZ5JjxUSeFqDZhT9qWJM5CBlckE7uvUOjSD1RWDNr+YFlacQaF22pZfD3eWfMfZ+6HBi67FVM4si4ml3A14dWqdtiDoFr62lXmVFCsSB6PzVjwRMdP/NBHQli0yB5ed1xaOZm+0zKT78W/Qw0qFRIHjuRqZ5uWD1jKLKbluQmqjOg5UW+b3ncPclDtxJGql7g1JkE0t5W4tOJ0aYxIP4dxMHG3D3WtJTTOiEQzHGiIyzUsDlwfBRYy0NtZZOR1m1DOvoTHZf2pz0zLY0KG0lAbA6+Cgr9lxykhbaqTZMlcuOPm7uN4YE0G4OVfHHUuwRh8Dd14Th+8md3RfgNfp9p5J0jt50Ex+PRLPeV3mA9+j1EjD6Uxt+Ch8Y/jmlBpptMtPgtb3yJ9rMggYnLYgcEai+Zl9JFagRsZ52TY6L+49XAjCxrH5pW6ZNxZnKZxOKrCUp1Pom7Rx/hinM6o10pSM+0WMtF1oPQO1p1P7CJy2EjHSDnEfFm1B4HxoNwD+C7M4Z04vzx1uZ1taypcj1oGlPNuUDu7ZRneJo7aYZ8lIG1nM1yW2BgMT3QGf5lKKVodKs1tbx8Bw6qI+8XdO3xgx0nbhZRWmbXZagn9a5x3Uzj6gOq3GWQTYOb688Rk4cwJG2q3F0WwN/XVVFw2pdnBPAK9TbB9w9sLOZd/M5rsPYOm9M76DwEZ+BKcuSnR/X6WjfIe4D4t9QehDU6HvRIUEA1CiUVNeo/HOYxAw0to0n60ZaV10XmN5T7fh8fCnXDgf3LnsmQUW7xVBv0/oclDjHIRns559ibitGY6XAzNfBYE897ieh1xu6ecOyub5Acp6pXSYp9Afl74h3A/Xd8zcMRP4LsABak/5cnwAxjLg8Y7vw+2+AfhHM1raR6ZkpA0clcEV8t4F35wa8GgJhu3lTx/LqW8Eoecphn3QiZZ2bpSe8h0yyp0gXDxjPz3BJof8EBbwF9OzbxQwQRAEwZB3gtCK6anruGObXx/MlFUQBGEUHPRUpgPoZlCRhVwGozlGfoZRwITLpfZdAEEYgs6jKyiyUAB1GFjfOKocrnEex2B3EXnKd8j39BHwGvJR8M9HMnj5cioBLSGlUOf0T6FcwiO4N/9bYLgYymPHptfl2CjBC5v5E8OZCT5DRckSs8TLJjjxd63ViiopypbPlQ0G38w+KQhtaLf9tZC0rNN0VhH9t/27PqJxHWbx6OMjDIQsnykeq6SQ0fpxaozPT8NX2+UYxgSuCtEHHwMRliDsQwUu6Z/loTQ0q4igRvxck8gpPKhkBzUjbWAx30sWBBl926O2eK+SkbZLhDgOYja/dV/tags7XsY+9m6Njr9mUSXFukqK+yopAqhNag6B/RL1omakvba4Mc49l+VsYM4Axzb6HRu1xXtZ61AZ30FtK0/YPaH01N9N8x3iPiycC0Ib2qTmRKsKHBXFlJqZPrKUL6cjrC3lOSTa03tf58SfCaWl+2gHhNHccNUa0FheHubc65RgcQZ2IghMckbawFEZjKCYAJypXNQ3T4MOcExLbLoMHUDlIqH2acPKrGSkDTTScGa4tvyfSp1ENHs5tazlY4Zu6z4sBhcEZqCVMa4vl4y0cwvLRnNO4jPdhC8ZaW9dOS6GWTy9AKfIfOB76HSWEeN+vYNKQRkf1JppI4002oJAAa36ivLGV4z5101l6rjm2EUaeoY6ojq3mSGzg3TRuQU9ry+hHzdiAjWiTU0yos6JE6PCl3d5X0pm+vswiyMbVhjUHhdoxSIm08EHAPMzNDm9R7+4ytzOKLKUBgBQJUUeZvECvM3ofVJG2kgjzYRp8ZiDZ0p96HovfAZe1wFrAH9jd0T1N6hjJGrL67a+14CjntdzlTsxGXVSR5UzL/MyqugLjeY4ZoI3sOCjQu9lDfXx7neg3wCU5+Yx3/IbMmWum5DqRmd/a8LsQ+Ywt7L5yTxOX7dcke4Nq6RIYX4e3BNd74XP9FJXOD6iuAbwm46ojvpkRsLD+ZBLzXScmcQ1jUCMoM6LE9ISAFacD4LCF5bgm+mepSAQOTP9XZjFa5PY3mEWB2EW5wD+QXcd3+AMPebJeMPk1OLvjjpTVloaiUfgi8KS46dC/ZnuN8YdyEbgi8ITPIdr/Qz1oDqVcotd7II5NyOq/BL6U9kto3Fyp/V/UzzomeEIMGemn0CJat7VgYVZfBVmcQolcNwp8/IMlzfamIjZDYB1mMWpjjDQHkEO4D+oIE06+J7RGlElxRzAn9DrVDcAvhosD3Pq5o7zrZEoTKE3+NoC+IuemQMn/S1n8NE6JFRXmJcArCyD9uEL+Ip0C1U591AdfAnVga3bD9PyYo6gGg63g+N0ECZ7DXf006wZP0I9y/2pl0LrnCn4I/gmBvUTVJnr1t8i9LOzT3tc650qKeowi5fQ76gbJlBLPsmRegVU3U7hKSyhL2gv4IpmpxHeb46WUN8tW4ypc+TGYGftp9FsPGodmTPFWxPQpt/JmeVoys9ta3Pwyv8MZViSYnfkT7v8NdQzrJjGNs74AvNN1glUg3htFJbP8ch1E1ZJ8UydQZ+NqGbvZEEblqdEZgHgt2FeN+hX1n2WY2lQPVlAfTimHbfter0IqMO3vZyYGlyThFmcc9vq/pE5lsgNrlmEWXxywLgPPe89zuBsts8Yp2frg0GcZW76Y0yg0VjoIxuDVc8WF2Kbb2FD1AXnvC/jBBqxc0fXDd7rk5a8TWbjE4yvfVrlM0bwgvbYwmwjz6b63mhuoM/h/7jpme91R5v02BB1wRM8mgCOmLzHtTe0j+MFA8OWfX5cgK/KUT7TdOyn74K0MNpYoWmZzRF7pJHnM9QSh5eDqMC3CjkXFvAXxrXhCWfmh2BicWWQR47+y3J3PkLlkhiU6L+XtHJtjkwGJoObPH8GADLV+g5/HRso7+89PW0XGPgZWiZyQ88Uvvd0GqwZaQftFFsWGlzzXls8QA1Mzs3rO3cZzIXEwHSpaJ9fZJgyCBbFAHQP1z4qJfStuKy989ejK6hzmcLPdH0D9QHmfW7SWCXAjiiUjHw5JnJ92QD4w4IHue5SofZBZzahMK4Rhp29bgH8WSXFOS/DlS5EwbIYNPwgM3bXo+0FgH9h18rsBm7rmuMDY800+s1ZRlVS1GTL+weGE4YMgLVAOJZG7E/cpZhWB+YyDvVPWKor2hTXWZZJ++bVB5q9foX7JaQMQODrDBmLNKNXK4YG5MS3hn0xaLgFUNsqb5swi6Mwi0uoExhc0IiCq7o+eW4XmRRbOxb+08vLS2cBodRnDrvmfFuojal7V+aSrTNq5uD5CzRrx8adbo+8D7GFGs2ntuuK3u8Kx9/t0sDZxxm07jyHvQ9gg107dDIjIBv0PufaPNJAQyevEm/r5hGq3ZTcTKltpHAnBIdo3gfbNLUNdZILDBs/4xGqHZn4dDT9xQLvZzFbKMOR8sB1M6j6sjXz+dopCHuZB1Aj7whqeYQrEI9QtsTl0KOwVtS2CMox5Apvy7+BWlNfwXIMXcp7Tnnr1tkGO6e/lcvliwOHGgKqLvKxbli3BiqNs5K29zuoDULVq/OlMM+C0LCBatsllCNXfeT6CKo+rY46DXnCrsx1l0C0yt2U3acDYruuy2PfbqvMEfQc/Jr6aJjBvs+NviAcgj7MgP7Z9iJ8xs6RpPNlfjRIIBpRatYf2/W1PuP1ay90tMMau81zL/U6EkE4RCOOYFwzBpp9ugD9Z99D0Qw49weiY6OfIAiC0M2IBUEQ9vnqI2KaIAiCMD6eRRAEQRAEVEmxFkEQBEEQtoCHmMqCIAjC6FgDIgiCIAgCmbSKIAiCIAgiCIIgCAIeG18xEQRBEISPTd78jwiCIAjCx+WxfXKyCIIgCMLH5c1JrSIIgiAIH5Ns/4BHEQRBEISPx7JKinT/lyIIgiAIH4ujcU5EEARBED4OnUGvRBAEQRAuny2Av05FQBRBEARBuGyWULHY708l/DJAYQRBEIRhMYrFLoIgCG6psQv7aAIn7rPzGNHCqGlC8Zam8dD/D5Kxgi2Fp/YiAAAAAElFTkSuQmCC'/>";
  ptr +="<div class='container'>";
  ptr +="<div class='data temperature'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Temperature</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)temperature;
  ptr +="<span class='superscript'>&deg;C</span></div>";
  ptr +="</div>";
  ptr +="<div class='data humidity'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 29.235 40.64'height=40.64px id=Layer_1 version=1.1 viewBox='0 0 29.235 40.64'width=29.235px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><path d='M14.618,0C14.618,0,0,17.95,0,26.022C0,34.096,6.544,40.64,14.618,40.64s14.617-6.544,14.617-14.617";
  ptr +="C29.235,17.95,14.618,0,14.618,0z M13.667,37.135c-5.604,0-10.162-4.56-10.162-10.162c0-0.787,0.638-1.426,1.426-1.426";
  ptr +="c0.787,0,1.425,0.639,1.425,1.426c0,4.031,3.28,7.312,7.311,7.312c0.787,0,1.425,0.638,1.425,1.425";
  ptr +="C15.093,36.497,14.455,37.135,13.667,37.135z'fill=#3C97D3 /></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Humidity</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)humidity;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";
  ptr +="<div class='data pressure'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 40.542 40.541'height=40.541px id=Layer_1 version=1.1 viewBox='0 0 40.542 40.541'width=40.542px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M34.313,20.271c0-0.552,0.447-1,1-1h5.178c-0.236-4.841-2.163-9.228-5.214-12.593l-3.425,3.424";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293c-0.391-0.391-0.391-1.023,0-1.414l3.425-3.424";
  ptr +="c-3.375-3.059-7.776-4.987-12.634-5.215c0.015,0.067,0.041,0.13,0.041,0.202v4.687c0,0.552-0.447,1-1,1s-1-0.448-1-1V0.25";
  ptr +="c0-0.071,0.026-0.134,0.041-0.202C14.39,0.279,9.936,2.256,6.544,5.385l3.576,3.577c0.391,0.391,0.391,1.024,0,1.414";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293L5.142,6.812c-2.98,3.348-4.858,7.682-5.092,12.459h4.804";
  ptr +="c0.552,0,1,0.448,1,1s-0.448,1-1,1H0.05c0.525,10.728,9.362,19.271,20.22,19.271c10.857,0,19.696-8.543,20.22-19.271h-5.178";
  ptr +="C34.76,21.271,34.313,20.823,34.313,20.271z M23.084,22.037c-0.559,1.561-2.274,2.372-3.833,1.814";
  ptr +="c-1.561-0.557-2.373-2.272-1.815-3.833c0.372-1.041,1.263-1.737,2.277-1.928L25.2,7.202L22.497,19.05";
  ptr +="C23.196,19.843,23.464,20.973,23.084,22.037z'fill=#26B999 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Pressure</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)pressure;
  ptr +="<span class='superscript'>hPa</span></div>";
  ptr +="</div>";
  ptr +="<div class='data altitude'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 58.422 40.639'height=40.639px id=Layer_1 version=1.1 viewBox='0 0 58.422 40.639'width=58.422px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M58.203,37.754l0.007-0.004L42.09,9.935l-0.001,0.001c-0.356-0.543-0.969-0.902-1.667-0.902";
  ptr +="c-0.655,0-1.231,0.32-1.595,0.808l-0.011-0.007l-0.039,0.067c-0.021,0.03-0.035,0.063-0.054,0.094L22.78,37.692l0.008,0.004";
  ptr +="c-0.149,0.28-0.242,0.594-0.242,0.934c0,1.102,0.894,1.995,1.994,1.995v0.015h31.888c1.101,0,1.994-0.893,1.994-1.994";
  ptr +="C58.422,38.323,58.339,38.024,58.203,37.754z'fill=#955BA5 /><path d='M19.704,38.674l-0.013-0.004l13.544-23.522L25.13,1.156l-0.002,0.001C24.671,0.459,23.885,0,22.985,0";
  ptr +="c-0.84,0-1.582,0.41-2.051,1.038l-0.016-0.01L20.87,1.114c-0.025,0.039-0.046,0.082-0.068,0.124L0.299,36.851l0.013,0.004";
  ptr +="C0.117,37.215,0,37.62,0,38.059c0,1.412,1.147,2.565,2.565,2.565v0.015h16.989c-0.091-0.256-0.149-0.526-0.149-0.813";
  ptr +="C19.405,39.407,19.518,39.019,19.704,38.674z'fill=#955BA5 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Altitude</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)altitude;
  ptr +="<span class='superscript'>m</span></div>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="</body>";
  ptr +="</html>";
  return ptr;

}
