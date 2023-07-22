import './style.css'
import './node_modules/bootstrap/dist/css/bootstrap.min.css'
import './node_modules/bootstrap/dist/js/bootstrap.min.js'
import mqtt from 'mqtt'
import * as JSC from 'jscharting'

// Constants
const MAX_VALUE = 5
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

// Variables
const placehoder = [{ x: 0, y: 0 }]
const humidityData = []
let count = 1
// const temperatureData = []

// Html elements
const humidityTable = document.getElementById('humidity-table-body')
const root = document.documentElement
// const temperatureTable = document.getElementById('temperature-table-body')

function addDataToChart (chart, points, label) {
  const options = {
    type: 'line',
    xAxis_label_text: 'Hora',
    yAxis_label_text: label,
    xAvis_scale_type: 'time',
    series: [{ name: label, points }]
  }

  JSC.Chart(chart, options)
}

function createTr (index, data) {
  const tr = document.createElement('tr')
  const th = document.createElement('th')
  const td1 = document.createElement('td')
  const td2 = document.createElement('td')

  th.setAttribute('scope', 'row')
  th.innerText = index
  td1.innerText = data.date.toDateString() + ' ' + data.date.toLocaleTimeString()
  td2.innerText = data.y

  tr.appendChild(th)
  tr.appendChild(td1)
  tr.appendChild(td2)

  return tr
}

function addData (data) {
  const date = new Date()
  const time = `${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`
  // const temperature = { x: time, y: data.t, date }
  const humidity = { x: time, y: data.h, date }

  let waterTop = -100 - data.h
  if (waterTop < -200) waterTop = -200
  console.log(waterTop)
  root.style.setProperty('--water-top', `${waterTop}%`)

  // temperatureData.push(temperature)
  humidityData.push(humidity)

  // if (temperatureData.length > MAX_VALUE) temperatureData.shift()
  if (humidityData.length > MAX_VALUE) {
    humidityData.shift()
    count++
  }

  // addDataToChart('temperature', temperatureData, 'Temperatura')
  addDataToChart('humidity', humidityData, 'Humedad')

  // Reset table
  humidityTable.innerHTML = ''
  // temperatureTable.innerHTML = ''

  humidityData.forEach((data, index) => {
    const tr = createTr(index + count, data)
    humidityTable.appendChild(tr)
  })

  /* temperatureData.forEach((data, index) => {
    const tr = createTr(index + 1, data)
    temperatureTable.appendChild(tr)
  }) */
}

function setup () {
  // addDataToChart('temperature', placehoder, 'Temperatura')
  addDataToChart('humidity', placehoder, 'Humedad')
  root.style.setProperty('--water-top', '-100%')

  const client = mqtt.connect(MQTT_BROKER, options)

  client.on('connect', function () {
    console.log('Connected!')
    client.subscribe(TOPIC)
  })

  client.on('disconnect', function () {
    console.log('disconnected')
  })

  client.on('message', function (_, message) {
    console.log(message.toString())
    const data = JSON.parse(message.toString())
    addData(data)
  })
}

setup()
