# MCUTrakingSoccerRobot

업로드된 코드는 그 이름에 따라 통상 경기, 승부차기, 승부차기 방어 로직.
그러나 승부차기와 승부차기 방어 로직은 환경을 고려한 코드가 없으므로, 통상 경기 로직에 집중하여 설명하도록 함.

## 프로젝트 목표
- 하드웨어 기기 주변 환경 고려, 특정 색 추적 이후 다음 차순위 태스크를 수행하는 마이너한 수준의 알고리즘 개발 지향
- 교내 MCU 기반 축구 대회에 참가, 4위 기록

## 사용한 부품
- PIXY 카메라, 초음파 센서(HR-SR04), Atmega128 MCU, Arduino board 등...

1. 색체 인식 및 트래킹 영상
url : https://www.youtube.com/shorts/D-zZEwbS4Z8

2. 통상 코드 업로드시 공 트래킹 및 수비 영상
url : https://youtu.be/0WiR46VJxNA

## 사용한 오픈소스 라이브러리

1. Avrdude

![image](https://user-images.githubusercontent.com/80696846/174485770-6edc566d-c01e-451f-9e70-b5aa753ec5dc.png)


- 본래 아두이노의 마이크로프로세서는 avr계열이므로, 코드 업로드를 위해  해당 라이브러리가 필요.
- 단, 업로드 이전에 usbasp를 통한 부트로더 업로드가 선행되어야 할 것.

2. Megacore

![image](https://user-images.githubusercontent.com/80696846/174485779-20d20092-f09f-48db-917e-79652069a8f7.png)

Atmega128은 기본적으로 아두이노 라이브러리와 호환되지 않음.
(추측으로는 아마 아두이노가 16비트를 표준으로 사용하고, 비트스트림이 맞지 않으면 유실이 생겨 해당 mcu를 타겟으로 만들지 않은 것으로 보임)
때문에 Atmega128 mcu를 아두이노에 장착하여 사용하기 위해서는 아두이노에 확장 라이브러리를 설치할 필요가 있음. 해당 open source library가 그 기능을 수행.

## 최종 완성 로봇 몸체 이미지

![image](https://user-images.githubusercontent.com/80696846/174485550-09274c27-421c-4f36-983a-ebfe8c22e833.png)

## 카메라 개체 인식 핵심 로직 및 영상

![image](https://user-images.githubusercontent.com/80696846/174485590-1af2dce8-5f69-4508-8364-c34c4e4f3b4d.png)

![image](https://user-images.githubusercontent.com/80696846/174485615-e2c8726c-1bbe-4647-bc23-d19864fb09e7.png)


pixy camera와 동일 bsp의 시리얼 통신을 받아들이고, Serial.read() 계열 함수를 이용하여 비트를 순차적으로 읽어냄.
이후 comport master 프로그램으로 추출한 좌표 데이터를 이용하여 dc 모터를 작동시키는 로직 작성

atmega128 프로세서와 arduino의 호환에서 생기로 보이는 이슈가 발생, x좌표의 상위 1비트가 0으로 고정되는 현상을 관측하였는데, 하위 1비트 데이터만을 이용하여 실제 좌표를 계산하는 로직이 필요하였음

## 인식 개체 좌표 보정 알고리즘

![image](https://user-images.githubusercontent.com/80696846/174485662-7c5bfcf8-64c4-401f-afc1-33c3960b4ec1.png)

curLoc 변수를 통해 이전까지 갱신되었던 좌표를 임시 저장하고, 새로 직렬 통신으로 받아들인 좌표 값을 X_center global variable에 저장
만일 이전까지 개체의 상태가 카메라의 우측단에 존재했었는데, 현재 갱신된 개체의 위치가 카메라 기준 좌측단으로 이동했다면 오버플로우가 발생한 것. (carry가 생성되는 과정에서 비트가 0으로 세팅되었을 가능성이 큼)

만일 위의 현상이 나타났다면, 오버플로우 발생 기점은 255에 현재 갱신된 x좌표를 더해 실제 좌표를 작성해낼 수 있음.

//초음파 센서와 관련해서는 추후 추가하기로 함.
