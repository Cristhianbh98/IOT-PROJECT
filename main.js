import './style.css'
import mqtt from 'mqtt'
import * as JSC from 'jscharting'

const MQTT_BROKER = 'ws://broker.hivemq.com:8000/mqtt'
const TOPIC = 'IOT/TEMHUM'
const CLIENT_ID = 'mqttjs_' + Math.random().toString(16).slice(3)
const options = {
  clientId: CLIENT_ID,
  clean: true,
  connectTimeout: 4000,
  username: '',
  password: '',
  reconnectPeriod: 1000
}

const placehoder = [{ x: 0, y: 0 }]
const temperatureData = []
const humidityData = []

function random (min, max) {
  min = Math.ceil(min)
  max = Math.floor(max)
  return Math.floor(Math.random() * (max - min + 1)) + min
}

function addDataToChart (chart, points, label) {
  JSC.Chart(chart, {
    type: 'line',
    xAxis_label_text: 'Hora',
    yAxis_label_text: label,
    xAvis_scale_type: 'time',
    series: [
      {
        name: label,
        points
      }
    ]
  })
}

function addData (data) {
  const date = new Date()
  const time = `${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`
  if (data.h <= 20) {
    data.t = random(23, 27)
  } else {
    data.t = random(18, 22)
  }
  const temperature = { x: time, y: data.t }
  const humidity = { x: time, y: data.h }
  temperatureData.push(temperature)
  humidityData.push(humidity)
  if (temperatureData.length > 10) temperatureData.shift()
  if (humidityData.length > 10) humidityData.shift()
  addDataToChart('temperature', temperatureData, 'Temperatura')
  addDataToChart('humidity', humidityData, 'Humedad')
}

function setup () {
  addDataToChart('temperature', placehoder, 'Temperatura')
  addDataToChart('humidity', placehoder, 'Humedad')

  const client = mqtt.connect(MQTT_BROKER, options)

  client.on('connect', function () {
    console.log('Connected!')
    client.subscribe(TOPIC)
  })

  client.on('disconnect', function () {
    console.log('disconnected')
  })

  client.on('message', function (topic, message) {
    console.log(message.toString())
    const data = JSON.parse(message.toString())
    addData(data)
  })
}

setup()
