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
  const [connectionStatus, setConnectionStatus] = useState('Not connected');
  const [values, setValues] = useState<number[]>([]);
  const [bucketSize, setBucketSize] = useState(200);

  useEffect(() => {
    const updateChart = () => {
      const counts: { [key: number]: number }  = {};
      values.forEach((val, idx)=> {
        if (idx == 0) { return }
        console.log(val, values[idx - 1])
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
      console.log("H1O");
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: ['00001234-0000-1000-8000-00805f9b34fb'] }]
      });
      const server = await device.gatt!.connect();
      const service = await server.getPrimaryService('00001234-0000-1000-8000-00805f9b34fb');
      const characteristic = await service.getCharacteristic('00001235-0000-1000-8000-00805f9b34fb');
      console.log("H2O");

      characteristic.addEventListener('characteristicvaluechanged', handleCharacteristicValueChanged);
      await characteristic.startNotifications();
      console.log("H3O");

      setConnectionStatus('Connected');
      setButtonDisabled(true);
      console.log("HO");
    } catch (error) {
      console.error('Error:', error);
      setConnectionStatus('Error: ' + error);
    }
  };

  const handleCharacteristicValueChanged = (event: Event) => {
    const value = new Uint32Array((event.target! as BluetoothRemoteGATTCharacteristic).value!.buffer).reverse()[0];
    setValues(prevValues => [...prevValues, value]);
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
            xs={6}
          >
            <Button onClick={connectToDevice} disabled={buttonDisabled}>
              Connect to BLE Peripheral
            </Button>
            <p id="connectionStatus" className={!buttonDisabled ? "disabled" : undefined}>Status: {connectionStatus}</p>
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