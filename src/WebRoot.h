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
    :root {
      --bg: #f3f6fb;
      --surface: #ffffff;
      --surface-muted: #eef3f9;
      --border: #d8e1ec;
      --text: #132033;
      --text-soft: #5d6b7d;
      --accent: #1f6feb;
      --accent-soft: #dce9ff;
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      padding: 1.5rem;
      font-family: system-ui, sans-serif;
      background: linear-gradient(180deg, #f8fbff 0%, var(--bg) 100%);
      color: var(--text);
    }
    #app { max-width: 900px; margin: 0 auto; }
    h1 { margin: 0 0 0.35rem; }
    .subtitle { margin: 0 0 1.25rem; color: var(--text-soft); }
    h2 { margin-top: 0; }
    .tab-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 0.6rem;
      margin-bottom: 1rem;
    }
    .tab-button {
      border: 1px solid var(--border);
      background: var(--surface);
      color: var(--text-soft);
      border-radius: 999px;
      padding: 0.6rem 1rem;
      margin-top: 0;
      cursor: pointer;
    }
    .tab-button.active {
      background: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }
    .tab-panel.hidden { display: none; }
    .card {
      padding: 1rem;
      border: 1px solid var(--border);
      background: var(--surface);
      border-radius: 14px;
      box-shadow: 0 10px 30px rgba(19, 32, 51, 0.06);
    }
    .stack { display: grid; gap: 1rem; }
    .alarm-card {
      margin-bottom: 1rem;
      padding: 0.75rem;
      border: 1px solid var(--border);
      border-radius: 10px;
      background: var(--surface-muted);
    }
    .status-grid {
      display: grid;
      gap: 0.75rem;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
    }
    .status-item {
      padding: 0.85rem;
      border-radius: 10px;
      background: var(--surface-muted);
    }
    .status-item strong {
      display: block;
      margin-bottom: 0.25rem;
      font-size: 0.85rem;
      color: var(--text-soft);
    }
    label { display: block; margin: 0.55rem 0; }
    input, select {
      width: 100%;
      padding: 0.55rem 0.65rem;
      margin: 0.35rem 0 0;
      border: 1px solid var(--border);
      border-radius: 8px;
      background: #fff;
    }
    .compact-grid {
      display: grid;
      gap: 0.85rem;
      grid-template-columns: repeat(auto-fit, minmax(120px, max-content));
      align-items: end;
    }
    .compact-grid label {
      margin: 0;
      min-width: 0;
    }
    .compact-grid input,
    .compact-grid select {
      width: auto;
      min-width: 0;
    }
    .time-grid input {
      width: 88px;
    }
    .timezone-grid select {
      width: min(320px, 100%);
    }
    .dst-mode-group {
      display: flex;
      flex-wrap: wrap;
      gap: 1rem;
      margin-top: 0.9rem;
    }
    .dst-mode-group label {
      display: flex;
      align-items: center;
      gap: 0.45rem;
      margin: 0;
    }
    .dst-mode-group input {
      width: auto;
      margin: 0;
    }
    .compact-section {
      max-width: 420px;
    }
    .alarm-tab-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 0.55rem;
      margin-bottom: 1rem;
    }
    .alarm-tab {
      border: 1px solid var(--border);
      background: var(--surface-muted);
      color: var(--text);
      border-radius: 999px;
      padding: 0.45rem 0.8rem;
      margin-top: 0;
      cursor: pointer;
    }
    .alarm-tab.active {
      background: var(--accent);
      border-color: var(--accent);
      color: #fff;
    }
    .alarm-editor-card {
      border: 1px solid var(--border);
      border-radius: 12px;
      background: var(--surface-muted);
      padding: 1rem;
    }
    .alarm-editor-grid {
      display: grid;
      gap: 0.85rem;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      align-items: end;
      width: 100%;
      max-width: 100%;
    }
    .alarm-editor-grid label {
      margin: 0;
      min-width: 0;
    }
    .alarm-title-field input {
      width: 100%;
    }
    .alarm-time-field input {
      width: 100%;
    }
    .alarm-select-field select {
      width: 100%;
    }
    .alarm-inline-check {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin: 0 0 0.25rem;
    }
    .alarm-inline-check input {
      width: auto;
      margin: 0;
    }
    button {
      padding: 0.55rem 0.9rem;
      margin-top: 0.5rem;
      border: 0;
      border-radius: 8px;
      background: var(--accent);
      color: #fff;
      cursor: pointer;
    }
    .secondary-button {
      background: var(--accent-soft);
      color: var(--accent);
    }
    .button-row {
      display: flex;
      flex-wrap: wrap;
      gap: 0.6rem;
      margin-top: 0.75rem;
    }
    #message-card {
      margin-top: 1rem;
      background: var(--accent-soft);
      border-color: #bfd3fb;
    }
    .hidden { display: none; }
    @media (max-width: 640px) {
      body { padding: 1rem; }
      .tab-bar { gap: 0.45rem; }
      .tab-button { flex: 1 1 calc(50% - 0.45rem); text-align: center; }
    }
  </style>
</head>
<body>
<div id="app">
  <h1>ESP32 LED Clock</h1>
  <p class="subtitle">Manage clock status and settings from one clean control panel.</p>

  <div class="tab-bar" role="tablist" aria-label="Clock pages">
    <button class="tab-button active" type="button" data-tab="status" aria-selected="true">Status</button>
    <button class="tab-button" type="button" data-tab="time" aria-selected="false">Set Time</button>
    <button class="tab-button" type="button" data-tab="timezone" aria-selected="false">Timezone</button>
    <button class="tab-button" type="button" data-tab="settings" aria-selected="false">Settings</button>
    <button class="tab-button" type="button" data-tab="alarms" aria-selected="false">Alarms</button>
    <button id="wifi-tab" class="tab-button hidden" type="button" data-tab="wifi" aria-selected="false">WiFi</button>
  </div>

  <section id="tab-status" class="tab-panel">
    <div class="card">
      <h2>Status</h2>
      <div id="loading">Loading...</div>
      <div id="status-content" class="hidden status-grid">
        <div class="status-item">
          <strong>Time</strong>
          <span id="status-time"></span>
        </div>
        <div class="status-item">
          <strong>Timezone</strong>
          <span id="status-timezone"></span>
        </div>
        <div class="status-item">
          <strong>Font</strong>
          <span id="status-font"></span>
        </div>
        <div class="status-item">
          <strong>DST</strong>
          <span id="status-dst"></span>
        </div>
        <div id="status-dst-offset-row" class="status-item hidden">
          <strong>DST Offset</strong>
          <span><span id="status-dst-offset"></span> h</span>
        </div>
        <div class="status-item">
          <strong>WiFi Mode</strong>
          <span id="status-wifi-mode"></span>
        </div>
        <div id="status-ap-ssid-row" class="status-item hidden">
          <strong>AP SSID</strong>
          <span id="status-ap-ssid"></span>
        </div>
        <div id="status-wifi-ssid-row" class="status-item hidden">
          <strong>SSID</strong>
          <span id="status-wifi-ssid"></span>
        </div>
        <div id="status-ip-row" class="status-item hidden">
          <strong>IP</strong>
          <span id="status-ip"></span>
        </div>
      </div>
    </div>
  </section>

  <section id="tab-time" class="tab-panel hidden">
    <div class="card">
      <h2>Set Time</h2>
      <div class="compact-grid time-grid compact-section">
        <label>Hour: <input id="time-hour" type="number" min="0" max="23"></label>
        <label>Minute: <input id="time-minute" type="number" min="0" max="59"></label>
        <label>Second: <input id="time-second" type="number" min="0" max="59"></label>
      </div>
      <div class="button-row">
        <button id="submit-time" type="button">Set Time</button>
      </div>
    </div>
  </section>

  <section id="tab-timezone" class="tab-panel hidden">
    <div class="card">
      <h2>Timezone</h2>
      <div class="compact-grid timezone-grid compact-section">
        <label>Timezone:
          <select id="timezone-select"></select>
        </label>
      </div>

      <div class="dst-mode-group">
        <label><input id="dst-auto" type="radio" name="dstmode" value="auto"> Automatic</label>
        <label><input id="dst-manual" type="radio" name="dstmode" value="manual"> Manual</label>
      </div>

      <div id="manual-dst-row" class="hidden compact-grid compact-section">
        <label>DST offset:
          <select id="manual-dst-select">
            <option value="0">0</option>
            <option value="3600">+1 hour</option>
            <option value="7200">+2 hours</option>
          </select>
        </label>
      </div>

      <div class="button-row">
        <button id="submit-timezone" type="button">Update Timezone</button>
      </div>
    </div>
  </section>

  <section id="tab-settings" class="tab-panel hidden">
    <div class="card">
      <h2>Settings</h2>
      <div class="compact-grid timezone-grid compact-section">
        <label>Digit font:
          <select id="font-select"></select>
        </label>
      </div>
      <div class="button-row">
        <button id="submit-settings" type="button">Save Settings</button>
      </div>
    </div>
  </section>

  <section id="tab-alarms" class="tab-panel hidden">
    <div class="card">
      <h2>Alarms</h2>
      <div id="alarms-empty">
        <p>No alarms configured.</p>
      </div>
      <div id="alarm-tabs" class="alarm-tab-bar"></div>
      <div id="alarm-editor"></div>
      <div class="button-row">
        <button id="add-alarm" class="secondary-button" type="button">Add Alarm</button>
        <button id="save-alarms" type="button">Save Alarms</button>
      </div>
    </div>
  </section>

  <section id="tab-wifi" class="tab-panel hidden">
    <div id="wifi-card" class="card hidden">
      <h2>WiFi (AP mode)</h2>
      <label>SSID: <input id="wifi-ssid" type="text"></label>
      <label>Password: <input id="wifi-password" type="password"></label>
      <div class="button-row">
        <button id="save-wifi" type="button">Save & Restart</button>
      </div>
    </div>
  </section>

  <div id="message-card" class="card hidden">
    <strong id="message-text"></strong>
  </div>
</div>

<script>
const state = {
  status: {},
  alarms: [],
  wifiEdited: false,
  alarmsEdited: false,
  activeTab: 'status',
  activeAlarmIndex: 0
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

function selectedFontIndex() {
  const availableFonts = Array.isArray(state.status.fonts) ? state.status.fonts.length : 1;
  return clampNumber(byId('font-select').value, 0, Math.max(availableFonts - 1, 0));
}

function setActiveTab(tabName) {
  state.activeTab = tabName;

  document.querySelectorAll('.tab-button').forEach((button) => {
    const active = button.dataset.tab === tabName;
    button.classList.toggle('active', active);
    button.setAttribute('aria-selected', active ? 'true' : 'false');
  });

  document.querySelectorAll('.tab-panel').forEach((panel) => {
    panel.classList.toggle('hidden', panel.id !== `tab-${tabName}`);
  });
}

function clampNumber(value, min, max) {
  const parsed = parseInt(value, 10);
  if (Number.isNaN(parsed)) return min;
  return Math.min(Math.max(parsed, min), max);
}

function defaultAlarmTitle(index) {
  return `Alarm ${index + 1}`;
}

function alarmTabLabel(alarm, index) {
  const title = (alarm.title || '').trim();
  return title || defaultAlarmTitle(index);
}

function isEditingAlarm() {
  const active = document.activeElement;
  return !!(active && active.closest && active.closest('#alarm-editor'));
}

function renderStatus(data) {
  state.status = data;

  byId('status-time').textContent = data.time || '';
  byId('status-timezone').textContent = data.timezone
    ? `${data.timezone.name} (${data.timezone.description})`
    : '';
  byId('status-font').textContent = data.font ? data.font.name : '';
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
  toggleHidden('wifi-tab', data.wifiMode !== 'AP');

  if (data.wifiMode !== 'AP' && state.activeTab === 'wifi') {
    setActiveTab('status');
  }

  if (!state.wifiEdited) {
    const currentSsid = data.wifiSsid || '';
    byId('wifi-ssid').value = currentSsid === 'Your_AP' ? '' : currentSsid;
    byId('wifi-password').value = '';
  }

  renderTimezoneControls(data);
  renderSettingsControls(data);
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
  const timezoneSelect = byId('timezone-select');
  timezoneSelect.innerHTML = '';

  (data.timezones || []).forEach((tz) => {
    const option = document.createElement('option');
    option.value = String(tz.index);
    option.textContent = `${tz.name} - ${tz.description}`;
    timezoneSelect.appendChild(option);
  });

  timezoneSelect.value = String(data.timezoneIndex || 0);

  const isManual = data.dstMode === 'manual';
  byId('dst-auto').checked = !isManual;
  byId('dst-manual').checked = isManual;
  byId('manual-dst-select').value = String(data.manualDstOffset || 0);
  toggleHidden('manual-dst-row', !isManual);
}

function renderSettingsControls(data) {
  const fontSelect = byId('font-select');
  fontSelect.innerHTML = '';
  (data.fonts || []).forEach((font) => {
    const option = document.createElement('option');
    option.value = String(font.index);
    option.textContent = font.name;
    fontSelect.appendChild(option);
  });
  fontSelect.value = String(data.fontIndex || 0);
}

function normalizeAlarm(alarm) {
  return {
    enabled: !!alarm.enabled,
    hour: clampNumber(alarm.hour, 0, 23),
    minute: clampNumber(alarm.minute, 0, 59),
    sound: clampNumber(alarm.sound, 0, 6),
    schedule: clampNumber(alarm.schedule, 0, 3),
    buttonAction: clampNumber(alarm.buttonAction, 0, 1),
    snoozeMinutes: clampNumber(alarm.snoozeMinutes, 1, 60),
    title: (alarm.title || '').trim()
  };
}

function updateAlarmSnoozeVisibility() {
  const action = clampNumber(byId('alarm-button-action').value, 0, 1);
  const showSnooze = action === 0;
  toggleHidden('alarm-snooze-minutes-row', !showSnooze);
  byId('alarm-snooze-minutes').disabled = !showSnooze;
}

function setActiveAlarm(index) {
  state.activeAlarmIndex = Math.min(Math.max(index, 0), Math.max(state.alarms.length - 1, 0));
  renderAlarms();
}

function updateAlarmTabLabel(index) {
  const tabs = document.querySelectorAll('#alarm-tabs .alarm-tab');
  if (!tabs[index] || !state.alarms[index]) {
    return;
  }
  tabs[index].textContent = alarmTabLabel(state.alarms[index], index);
}

function updateAlarmField(index, key, value, rerender = false) {
  if (!state.alarms[index]) {
    return;
  }
  state.alarmsEdited = true;
  state.alarms[index][key] = value;
  if (key === 'title') {
    updateAlarmTabLabel(index);
  }
  if (rerender) {
    renderAlarms();
  }
}

function removeAlarm(index) {
  state.alarmsEdited = true;
  state.alarms.splice(index, 1);
  if (state.activeAlarmIndex >= state.alarms.length) {
    state.activeAlarmIndex = Math.max(state.alarms.length - 1, 0);
  }
  renderAlarms();
}

function renderAlarms() {
  const tabs = byId('alarm-tabs');
  const editor = byId('alarm-editor');
  tabs.innerHTML = '';
  editor.innerHTML = '';
  toggleHidden('alarms-empty', state.alarms.length !== 0);
  toggleHidden('alarm-tabs', state.alarms.length === 0);

  if (state.alarms.length === 0) {
    return;
  }

  state.activeAlarmIndex = Math.min(state.activeAlarmIndex, state.alarms.length - 1);
  state.alarms.forEach((alarm, index) => {
    const button = document.createElement('button');
    button.type = 'button';
    button.className = 'alarm-tab';
    if (index === state.activeAlarmIndex) {
      button.classList.add('active');
    }
    button.textContent = alarmTabLabel(alarm, index);
    button.addEventListener('click', () => setActiveAlarm(index));
    tabs.appendChild(button);
  });

  const alarm = state.alarms[state.activeAlarmIndex];
  const wrapper = document.createElement('div');
  wrapper.className = 'alarm-editor-card';
  wrapper.innerHTML = `
    <label class="alarm-inline-check"><input id="alarm-enabled" type="checkbox"> Enabled</label>
    <div class="alarm-editor-grid">
      <label class="alarm-title-field">Title: <input id="alarm-title" type="text" maxlength="24"></label>
      <label class="alarm-time-field">Hour: <input id="alarm-hour" type="number" min="0" max="23"></label>
      <label class="alarm-time-field">Minute: <input id="alarm-minute" type="number" min="0" max="59"></label>
      <label class="alarm-select-field">Days:
        <select id="alarm-schedule">
          <option value="0">Every day</option>
          <option value="1">Weekdays</option>
          <option value="2">Weekends</option>
          <option value="3">Single event</option>
        </select>
      </label>
      <label class="alarm-select-field">Sound:
        <select id="alarm-sound">
          <option value="0">Siren</option>
          <option value="1">Beep</option>
          <option value="2">Fanfare</option>
          <option value="3">Nokia tune</option>
          <option value="4">SMS chirp</option>
          <option value="5">Motorola ring</option>
          <option value="6">Siemens tone</option>
        </select>
      </label>
      <label class="alarm-select-field">Snooze button:
        <select id="alarm-button-action">
          ${(state.status.snoozeButtonActions || []).map((action) => `<option value="${action.index}">${action.name}</option>`).join('')}
        </select>
      </label>
      <label id="alarm-snooze-minutes-row" class="alarm-time-field">Snooze minutes:
        <input id="alarm-snooze-minutes" type="number" min="1" max="60">
      </label>
    </div>
    <div class="button-row">
      <button id="test-alarm" type="button">Test sound</button>
      <button id="remove-alarm" class="secondary-button" type="button">Remove</button>
    </div>
  `;
  editor.appendChild(wrapper);

  byId('alarm-enabled').checked = alarm.enabled;
  byId('alarm-title').value = alarm.title || defaultAlarmTitle(state.activeAlarmIndex);
  byId('alarm-hour').value = alarm.hour;
  byId('alarm-minute').value = alarm.minute;
  byId('alarm-sound').value = String(alarm.sound);
  byId('alarm-schedule').value = String(alarm.schedule);
  byId('alarm-button-action').value = String(alarm.buttonAction);
  byId('alarm-snooze-minutes').value = String(alarm.snoozeMinutes);
  updateAlarmSnoozeVisibility();

  byId('alarm-enabled').addEventListener('change', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'enabled', event.target.checked);
  });
  byId('alarm-title').addEventListener('input', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'title', event.target.value.slice(0, 24));
  });
  byId('alarm-hour').addEventListener('input', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'hour', clampNumber(event.target.value, 0, 23));
  });
  byId('alarm-minute').addEventListener('input', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'minute', clampNumber(event.target.value, 0, 59));
  });
  byId('alarm-sound').addEventListener('change', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'sound', clampNumber(event.target.value, 0, 6));
  });
  byId('alarm-schedule').addEventListener('change', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'schedule', clampNumber(event.target.value, 0, 3));
  });
  byId('alarm-button-action').addEventListener('change', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'buttonAction', clampNumber(event.target.value, 0, 1));
    updateAlarmSnoozeVisibility();
  });
  byId('alarm-snooze-minutes').addEventListener('input', (event) => {
    updateAlarmField(state.activeAlarmIndex, 'snoozeMinutes', clampNumber(event.target.value, 1, 60));
  });
  byId('test-alarm').addEventListener('click', () => {
    testAlarmSound(state.activeAlarmIndex);
  });
  byId('remove-alarm').addEventListener('click', () => {
    removeAlarm(state.activeAlarmIndex);
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
    if (!state.alarmsEdited && !isEditingAlarm()) {
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
      font: state.status.fontIndex || 0,
      dstmode: selectedDstMode(),
      manualdst: clampNumber(byId('manual-dst-select').value, 0, 7200)
    });
    setMessage(data.message || 'Timezone updated');
    await loadStatus();
  } catch (error) {
    setMessage(error.message || 'Failed to update timezone');
  }
}

async function submitSettings() {
  try {
    const data = await postForm('/api/settings', {
      font: selectedFontIndex()
    });
    setMessage(data.message || 'Settings updated');
    await loadStatus();
  } catch (error) {
    setMessage(error.message || 'Failed to update settings');
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
      const title = encodeURIComponent((alarm.title || '').trim());
      return `${enabled},${alarm.hour},${alarm.minute},${alarm.sound},${alarm.schedule},${alarm.buttonAction},${alarm.snoozeMinutes},${title}`;
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
    title: defaultAlarmTitle(state.alarms.length),
    hour: 0,
    minute: 0,
    sound: 0,
    schedule: 0,
    buttonAction: 0,
    snoozeMinutes: 10
  });
  state.activeAlarmIndex = state.alarms.length - 1;
  renderAlarms();
}

function init() {
  document.querySelectorAll('.tab-button').forEach((button) => {
    button.addEventListener('click', () => setActiveTab(button.dataset.tab));
  });
  byId('submit-time').addEventListener('click', submitTime);
  byId('submit-timezone').addEventListener('click', submitTimezone);
  byId('submit-settings').addEventListener('click', submitSettings);
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

  setActiveTab('status');
  loadStatus();
  setInterval(loadStatus, 5000);
}

init();
</script>
</body>
</html>
)rawliteral";

#endif // WEBROOT_H
