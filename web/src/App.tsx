/// <reference types="web-bluetooth" />

import "./App.css"

import React, { useState, useEffect, ChangeEvent } from 'react';
import Highcharts from 'highcharts';
import HighchartsReact from 'highcharts-react-official';

import DarkUnica from 'highcharts/themes/dark-unica';
DarkUnica(Highcharts);

import '@fontsource/roboto/300.css';
import '@fontsource/roboto/400.css';
import '@fontsource/roboto/500.css';
import '@fontsource/roboto/700.css';

import Grid from "@mui/material/Grid";
import Button from '@mui/material/Button';
import Input from '@mui/material/Input';

const HistogramPlot = () => {
  const [chartOptions, setChartOptions] = useState<Highcharts.Options>({
    chart: {
      type: 'column'
    },
    title: {
      text: 'Histogram Plot'
    },
    xAxis: {
      categories: [],
      title: {
        text: 'Value'
      }
    },
    yAxis: {
      title: {
        text: 'Frequency'
      }
    },
    series: [{
      type: 'column',
      name: 'Value',
      data: []
    }]
  });

  const [buttonDisabled, setButtonDisabled] = useState(false);
  const [connectionStatus, setConnectionStatus] = useState('NOT CONNECTED');
  const [values, setValues] = useState<number[]>([]);
  const [bucketSize, setBucketSize] = useState(200);
  const [battery, setBattery] = useState('NOT CONNECTED');

  useEffect(() => {
    const updateChart = () => {
      const counts: { [key: number]: number }  = {};
      values.forEach((val, idx)=> {
        if (idx == 0) { return }
        const bucket = Math.floor((val - values[idx - 1]) / bucketSize) * bucketSize;
        counts[bucket] = counts[bucket] ? counts[bucket] + 1 : 1;
      });

      const categories = Object.keys(counts).map(Number).sort((a, b) => a - b);
      const data = categories.map(category => counts[category]);

      setChartOptions(prevOptions => ({
        ...prevOptions,
        xAxis: {
          categories: categories.map(String),
          title: {
            text: 'Value'
          }
        },
        series: [{
          type: 'bar',
          name: 'Value',
          data
        }]
      }));
    };

    updateChart();
  }, [values, bucketSize]);

  const connectToDevice = async () => {
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: ['00001234-0000-1000-8000-00805f9b34fb'] }]
      });
      const server = await device.gatt!.connect();
      const service = await server.getPrimaryService('00001234-0000-1000-8000-00805f9b34fb');

      const buttonCharacteristic = await service.getCharacteristic('00001235-0000-1000-8000-00805f9b34fb');
      buttonCharacteristic.addEventListener('characteristicvaluechanged', handleCharacteristicValueChanged);
      await buttonCharacteristic.startNotifications();

      const batteryCharacteristic = await service.getCharacteristic('00001236-0000-1000-8000-00805f9b34fb');
      batteryCharacteristic.addEventListener('characteristicvaluechanged', handleBatteryCharacteristicValueChanged);
      await batteryCharacteristic.startNotifications();

      setConnectionStatus('CONNECTED');
      setButtonDisabled(true);

      device.addEventListener('gattserverdisconnected', onDisconnected);

      const batteryValue = await batteryCharacteristic.readValue();
      const decoder = new TextDecoder('utf-8');
      const stringValue = decoder.decode(batteryValue);
      setBattery(stringValue + " Volts");

    } catch (error) {
      console.error('Error:', error);
      setConnectionStatus('Error: ' + error);
    }
  };

  // Function called when the device is disconnected
  function onDisconnected(_event: Event) {
      setConnectionStatus('NOT CONNECTED');
      setButtonDisabled(false);
  }

  const handleCharacteristicValueChanged = (event: Event) => {
    const value = new Uint32Array((event.target! as BluetoothRemoteGATTCharacteristic).value!.buffer).reverse()[0];
    setValues(prevValues => [...prevValues, value]);
  };

  const handleBatteryCharacteristicValueChanged = (event: Event) => {
    const batteryValue = (event.target! as BluetoothRemoteGATTCharacteristic).value!;
    const decoder = new TextDecoder('utf-8');
    const stringValue = decoder.decode(batteryValue);

    setBattery(stringValue + " Volts");
  };

  const handleBucketSizeChange = (event: ChangeEvent & {target: HTMLInputElement | HTMLTextAreaElement}) => {
    setBucketSize(parseInt(event.target!.value, 10));
  };

  return (
    <div>
      <Grid container
          spacing={2}
          justifyContent="space-between"
        >
          <Grid item
            xs={3}
          >
            <Button onClick={connectToDevice} disabled={buttonDisabled}>
              Connect to BLE Peripheral
            </Button>
            <p id="connectionStatus" className={!buttonDisabled ? "disabled" : undefined}>Status: {connectionStatus}</p>
          </Grid>

          <Grid item
            xs={3}
          >
            <p id="battery" className={!buttonDisabled ? "disabled" : undefined}>🔋: {battery}</p>
          </Grid>

          <Grid item
            xs={6}
          >
            <label htmlFor="bucketSize">Bucket Size:&nbsp;</label>
            <Input
              type="number"
              id="bucketSize"
              name="bucketSize"
              value={bucketSize}
              onChange={handleBucketSizeChange}
              sx={{
                min: "1",
                step: "10",
              }}
            />
          </Grid>

          <Grid item
            xs={12}
          >
            <HighchartsReact highcharts={Highcharts} options={chartOptions} />
          </Grid>

      </Grid>
    </div>
  );
};

export default HistogramPlot;