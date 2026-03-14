#ifndef WEBROOT_H
#define WEBROOT_H

#include <pgmspace.h>

static const char index_html[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 LED Clock</title>
  <style>
    body { font-family: system-ui, sans-serif; margin: 1rem; }
    h1 { margin-bottom: 0.5rem; }
    h2 { margin-top: 0; }
    .card { padding: 1rem; border: 1px solid #ccc; border-radius: 10px; margin-bottom: 1rem; }
    .alarm-card { margin-bottom: 1rem; padding: 0.5rem; border: 1px solid #ddd; border-radius: 8px; }
    label { display: block; margin: 0.4rem 0; }
    input, select { padding: 0.3rem; margin-left: 0.4rem; }
    button { padding: 0.4rem 0.7rem; margin-top: 0.5rem; }
    .hidden { display: none; }
  </style>
</head>
<body>
<div id="app">
  <h1>ESP32 LED Clock</h1>

  <div class="card">
    <h2>Status</h2>
    <div id="loading">Loading...</div>
    <div id="status-content" class="hidden">
      <p><strong>Time:</strong> <span id="status-time"></span></p>
      <p><strong>Timezone:</strong> <span id="status-timezone"></span></p>
      <p><strong>DST:</strong> <span id="status-dst"></span></p>
      <p id="status-dst-offset-row" class="hidden"><strong>DST Offset:</strong> <span id="status-dst-offset"></span> h</p>
      <p><strong>WiFi mode:</strong> <span id="status-wifi-mode"></span></p>
      <p id="status-ap-ssid-row" class="hidden"><strong>AP SSID:</strong> <span id="status-ap-ssid"></span></p>
      <p id="status-wifi-ssid-row" class="hidden"><strong>SSID:</strong> <span id="status-wifi-ssid"></span></p>
      <p id="status-ip-row" class="hidden"><strong>IP:</strong> <span id="status-ip"></span></p>
    </div>
  </div>

  <div class="card">
    <h2>Set Time</h2>
    <label>Hour: <input id="time-hour" type="number" min="0" max="23"></label>
    <label>Minute: <input id="time-minute" type="number" min="0" max="59"></label>
    <label>Second: <input id="time-second" type="number" min="0" max="59"></label>
    <button id="submit-time" type="button">Set Time</button>
  </div>

  <div class="card">
    <h2>Timezone</h2>
    <label>Timezone:
      <select id="timezone-select"></select>
    </label>

    <div>
      <label><input id="dst-auto" type="radio" name="dstmode" value="auto"> Automatic</label>
      <label><input id="dst-manual" type="radio" name="dstmode" value="manual"> Manual</label>
    </div>

    <div id="manual-dst-row" class="hidden">
      <label>DST offset:
        <select id="manual-dst-select">
          <option value="0">0</option>
          <option value="3600">+1 hour</option>
          <option value="7200">+2 hours</option>
        </select>
      </label>
    </div>

    <button id="submit-timezone" type="button">Update Timezone</button>
  </div>

  <div class="card">
    <h2>Alarms</h2>
    <div id="alarms-empty">
      <p>No alarms configured.</p>
    </div>
    <div id="alarms-list"></div>
    <button id="add-alarm" type="button" style="margin-right:0.5rem;">Add Alarm</button>
    <button id="save-alarms" type="button">Save Alarms</button>
  </div>

  <div id="wifi-card" class="card hidden">
    <h2>WiFi (AP mode)</h2>
    <label>SSID: <input id="wifi-ssid" type="text"></label>
    <label>Password: <input id="wifi-password" type="password"></label>
    <button id="save-wifi" type="button">Save & Restart</button>
  </div>

  <div id="message-card" class="card hidden">
    <strong id="message-text"></strong>
  </div>
</div>

<template id="alarm-template">
  <div class="alarm-card">
    <label><input class="alarm-enabled" type="checkbox"> Enabled</label>
    <label>Hour: <input class="alarm-hour" type="number" min="0" max="23"></label>
    <label>Minute: <input class="alarm-minute" type="number" min="0" max="59"></label>
    <label>Days:
      <select class="alarm-schedule">
        <option value="0">Every day</option>
        <option value="1">Weekdays</option>
        <option value="2">Weekends</option>
      </select>
    </label>
    <label>Sound:
      <select class="alarm-sound">
        <option value="0">Siren</option>
        <option value="1">Beep</option>
        <option value="2">Fanfare</option>
        <option value="3">Nokia tune</option>
        <option value="4">SMS chirp</option>
        <option value="5">Motorola ring</option>
        <option value="6">Siemens tone</option>
      </select>
    </label>
    <button class="test-alarm" type="button">Test sound</button>
    <button class="remove-alarm" type="button">Remove</button>
  </div>
</template>

<script>
const state = {
  status: {},
  alarms: [],
  wifiEdited: false,
  alarmsEdited: false
};

function byId(id) {
  return document.getElementById(id);
}

function setMessage(text) {
  const card = byId('message-card');
  byId('message-text').textContent = text || '';
  card.classList.toggle('hidden', !text);
}

function toggleHidden(id, hidden) {
  byId(id).classList.toggle('hidden', hidden);
}

function selectedDstMode() {
  return byId('dst-manual').checked ? 'manual' : 'auto';
}

function clampNumber(value, min, max) {
  const parsed = parseInt(value, 10);
  if (Number.isNaN(parsed)) return min;
  return Math.min(Math.max(parsed, min), max);
}

function renderStatus(data) {
  state.status = data;

  byId('status-time').textContent = data.time || '';
  byId('status-timezone').textContent = data.timezone
    ? `${data.timezone.name} (${data.timezone.description})`
    : '';
  byId('status-dst').textContent = data.dstMode || '';
  byId('status-dst-offset').textContent = String((data.manualDstOffset || 0) / 3600);
  byId('status-wifi-mode').textContent = data.wifiMode || '';
  byId('status-ap-ssid').textContent = data.apSsid || '';
  byId('status-wifi-ssid').textContent = data.wifiSsid || '';
  byId('status-ip').textContent = data.ip || '';

  toggleHidden('status-dst-offset-row', data.dstMode !== 'manual');
  toggleHidden('status-ap-ssid-row', data.wifiMode !== 'AP');
  toggleHidden('status-wifi-ssid-row', data.wifiMode !== 'station');
  toggleHidden('status-ip-row', !data.ip);
  toggleHidden('wifi-card', data.wifiMode !== 'AP');

  if (!state.wifiEdited) {
    const currentSsid = data.wifiSsid || '';
    byId('wifi-ssid').value = currentSsid === 'Your_AP' ? '' : currentSsid;
    byId('wifi-password').value = '';
  }

  renderTimezoneControls(data);
}

function syncTimeControls(data) {
  const active = document.activeElement;
  if (active !== byId('time-hour')) {
    byId('time-hour').value = data.hour || 0;
  }
  if (active !== byId('time-minute')) {
    byId('time-minute').value = data.minute || 0;
  }
  if (active !== byId('time-second')) {
    byId('time-second').value = data.second || 0;
  }
}

function renderTimezoneControls(data) {
  const select = byId('timezone-select');
  select.innerHTML = '';

  (data.timezones || []).forEach((tz) => {
    const option = document.createElement('option');
    option.value = String(tz.index);
    option.textContent = `${tz.name} - ${tz.description}`;
    select.appendChild(option);
  });

  select.value = String(data.timezoneIndex || 0);
  const isManual = data.dstMode === 'manual';
  byId('dst-auto').checked = !isManual;
  byId('dst-manual').checked = isManual;
  byId('manual-dst-select').value = String(data.manualDstOffset || 0);
  toggleHidden('manual-dst-row', !isManual);
}

function normalizeAlarm(alarm) {
  return {
    enabled: !!alarm.enabled,
    hour: clampNumber(alarm.hour, 0, 23),
    minute: clampNumber(alarm.minute, 0, 59),
    sound: clampNumber(alarm.sound, 0, 6),
    schedule: clampNumber(alarm.schedule, 0, 2)
  };
}

function renderAlarms() {
  const list = byId('alarms-list');
  list.innerHTML = '';
  toggleHidden('alarms-empty', state.alarms.length !== 0);

  state.alarms.forEach((alarm, index) => {
    const node = byId('alarm-template').content.firstElementChild.cloneNode(true);
    node.querySelector('.alarm-enabled').checked = alarm.enabled;
    node.querySelector('.alarm-hour').value = alarm.hour;
    node.querySelector('.alarm-minute').value = alarm.minute;
    node.querySelector('.alarm-sound').value = String(alarm.sound);
    node.querySelector('.alarm-schedule').value = String(alarm.schedule);

    node.querySelector('.alarm-enabled').addEventListener('change', (event) => {
      state.alarmsEdited = true;
      state.alarms[index].enabled = event.target.checked;
    });
    node.querySelector('.alarm-hour').addEventListener('input', (event) => {
      state.alarmsEdited = true;
      state.alarms[index].hour = clampNumber(event.target.value, 0, 23);
    });
    node.querySelector('.alarm-minute').addEventListener('input', (event) => {
      state.alarmsEdited = true;
      state.alarms[index].minute = clampNumber(event.target.value, 0, 59);
    });
    node.querySelector('.alarm-sound').addEventListener('change', (event) => {
      state.alarmsEdited = true;
      state.alarms[index].sound = clampNumber(event.target.value, 0, 6);
    });
    node.querySelector('.alarm-schedule').addEventListener('change', (event) => {
      state.alarmsEdited = true;
      state.alarms[index].schedule = clampNumber(event.target.value, 0, 2);
    });
    node.querySelector('.remove-alarm').addEventListener('click', () => {
      state.alarmsEdited = true;
      state.alarms.splice(index, 1);
      renderAlarms();
    });
    node.querySelector('.test-alarm').addEventListener('click', () => {
      testAlarmSound(index);
    });

    list.appendChild(node);
  });
}

async function postForm(url, values) {
  const body = new URLSearchParams();
  Object.entries(values).forEach(([key, value]) => body.append(key, String(value)));

  const response = await fetch(url, { method: 'POST', body });
  const data = await response.json();
  if (!response.ok || !data.success) {
    throw new Error(data.message || 'Request failed');
  }
  return data;
}

async function loadStatus() {
  toggleHidden('loading', false);
  toggleHidden('status-content', true);

  try {
    const response = await fetch('/api/status');
    const data = await response.json();
    renderStatus(data);
    syncTimeControls(data);
    if (!state.alarmsEdited) {
      state.alarms = Array.isArray(data.alarms) ? data.alarms.map(normalizeAlarm) : [];
      renderAlarms();
    }
    toggleHidden('status-content', false);
    toggleHidden('loading', true);
  } catch (error) {
    setMessage('Failed to load status');
  }
}

async function submitTime() {
  try {
    const data = await postForm('/api/settime', {
      hour: clampNumber(byId('time-hour').value, 0, 23),
      minute: clampNumber(byId('time-minute').value, 0, 59),
      second: clampNumber(byId('time-second').value, 0, 59)
    });
    setMessage(data.message || 'Time updated');
    await loadStatus();
  } catch (error) {
    setMessage(error.message || 'Failed to update time');
  }
}

async function submitTimezone() {
  try {
    const data = await postForm('/api/timezone', {
      timezone: clampNumber(byId('timezone-select').value, 0, 99),
      dstmode: selectedDstMode(),
      manualdst: clampNumber(byId('manual-dst-select').value, 0, 7200)
    });
    setMessage(data.message || 'Timezone updated');
    await loadStatus();
  } catch (error) {
    setMessage(error.message || 'Failed to update timezone');
  }
}

async function submitWifi() {
  const ssid = byId('wifi-ssid').value.trim();
  const password = byId('wifi-password').value;

  if (!ssid) {
    setMessage('SSID is required');
    return;
  }

  try {
    const data = await postForm('/api/wifi', { ssid, password });
    setMessage(data.message || 'Saved - restarting');
  } catch (error) {
    setMessage(error.message || 'Failed to save WiFi');
  }
}

async function submitAlarms() {
  try {
    const encoded = state.alarms.map((alarm) => {
      const enabled = alarm.enabled ? '1' : '0';
      return `${enabled},${alarm.hour},${alarm.minute},${alarm.sound},${alarm.schedule}`;
    }).join('|');
    const data = await postForm('/api/alarms', { alarms: encoded });
    state.alarmsEdited = false;
    setMessage(data.message || 'Alarms saved');
    await loadStatus();
  } catch (error) {
    setMessage(error.message || 'Failed to save alarms');
  }
}

async function testAlarmSound(index) {
  try {
    const data = await postForm('/api/alarm/test', {
      sound: clampNumber(state.alarms[index].sound, 0, 6)
    });
    setMessage(data.message || 'Sound played');
  } catch (error) {
    setMessage(error.message || 'Failed to play sound');
  }
}

function addAlarm() {
  if (state.alarms.length >= 6) {
    setMessage('Maximum of 6 alarms');
    return;
  }

  state.alarmsEdited = true;
  state.alarms.push({
    enabled: false,
    hour: 0,
    minute: 0,
    sound: 0,
    schedule: 0
  });
  renderAlarms();
}

function init() {
  byId('submit-time').addEventListener('click', submitTime);
  byId('submit-timezone').addEventListener('click', submitTimezone);
  byId('save-wifi').addEventListener('click', submitWifi);
  byId('save-alarms').addEventListener('click', submitAlarms);
  byId('add-alarm').addEventListener('click', addAlarm);
  byId('wifi-ssid').addEventListener('input', () => {
    state.wifiEdited = true;
  });
  byId('wifi-password').addEventListener('input', () => {
    state.wifiEdited = true;
  });
  byId('dst-auto').addEventListener('change', () => toggleHidden('manual-dst-row', true));
  byId('dst-manual').addEventListener('change', () => toggleHidden('manual-dst-row', false));

  loadStatus();
  setInterval(loadStatus, 5000);
}

init();
</script>
</body>
</html>
)rawliteral";

#endif // WEBROOT_H
