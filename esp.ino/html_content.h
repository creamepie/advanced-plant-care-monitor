#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

const char HTML_CONTENT[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <!-- Enhanced viewport settings for mobile -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
    <!-- Improved full-screen settings for iOS -->
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
    <meta name="mobile-web-app-capable" content="yes">
    <!-- Add home screen app name -->
    <meta name="apple-mobile-web-app-title" content="Plant Care">
    <link rel="manifest" href="/manifest.json">
    <title>Plant Care Dashboard</title>
    <style>
        :root {
            /* Electric Nature Theme - Light */
            --bg-color-light: #f0f9ff; /* Soft sky blue */
            --card-bg-light: #ffffff;
            --text-color-light: #1e293b; /* Dark slate */
            
            /* Gradient fills for gauges and buttons */
            --primary-gradient-light: linear-gradient(135deg, #10b981, #06b6d4); /* Teal to cyan */
            --secondary-gradient-light: linear-gradient(135deg, #3b82f6, #8b5cf6); /* Blue to purple */
            --accent-gradient-light: linear-gradient(135deg, #f59e0b, #ef4444); /* Amber to red */
            
            /* Solid colors */
            --primary-color-light: #10b981; /* Emerald */
            --secondary-color-light: #3b82f6; /* Blue */
            --accent-color-light: #f59e0b; /* Amber */
            --success-color-light: #22c55e; /* Green */
            --danger-color-light: #ef4444; /* Red */
            
            --border-color-light: rgba(226, 232, 240, 0.8);
            --shadow-light: 0 10px 25px -5px rgba(0, 0, 0, 0.1), 0 8px 10px -6px rgba(0, 0, 0, 0.06);
            
            --gauge-bg-light: #e2e8f0; /* Light slate */
            --gauge-fill-light: var(--primary-gradient-light);
            --button-bg-light: rgba(255, 255, 255, 0.8);
            --button-active-bg-light: var(--primary-gradient-light);
            --button-active-text-light: #fff;
            
            /* Electric Nature Theme - Dark */
            --bg-color-dark:rgb(0, 0, 0); /* Dark slate */
            --card-bg-dark: #1e293b; /* Slightly lighter slate */
            --text-color-dark: #f8fafc; /* Off white */
            
            /* Gradient fills for gauges and buttons */
            --primary-gradient-dark: linear-gradient(135deg, #10b981, #0ea5e9); /* Emerald to sky */
            --secondary-gradient-dark: linear-gradient(135deg, #6366f1, #d946ef); /* Indigo to fuchsia */
            --accent-gradient-dark: linear-gradient(135deg, #eab308, #f43f5e); /* Yellow to rose */
            
            /* Solid colors */
            --primary-color-dark: #10b981; /* Emerald */
            --secondary-color-dark: #6366f1; /* Indigo */
            --accent-color-dark: #eab308; /* Yellow */
            --success-color-dark: #22c55e; /* Green */
            --danger-color-dark: #f43f5e; /* Rose */
            
            --border-color-dark: rgba(51, 65, 85, 0.8);
            --shadow-dark: 0 10px 25px -5px rgba(0, 0, 0, 0.5), 0 8px 10px -6px rgba(0, 0, 0, 0.2);
            
            --gauge-bg-dark: #334155; /* Slate 700 */
            --gauge-fill-dark: var(--primary-gradient-dark);
            --button-bg-dark: rgba(30, 41, 59, 0.8);
            --button-active-bg-dark: var(--primary-gradient-dark);
            --button-active-text-dark: #f8fafc;
            
            /* Default to light mode */
            --bg-color: var(--bg-color-light);
            --card-bg: var(--card-bg-light);
            --text-color: var(--text-color-light);
            --primary-color: var(--primary-color-light);
            --secondary-color: var(--secondary-color-light);
            --accent-color: var(--accent-color-light);
            --success-color: var(--success-color-light);
            --danger-color: var(--danger-color-light);
            --primary-gradient: var(--primary-gradient-light);
            --secondary-gradient: var(--secondary-gradient-light);
            --accent-gradient: var(--accent-gradient-light);
            --border-color: var(--border-color-light);
            --shadow: var(--shadow-light);
            --gauge-bg: var(--gauge-bg-light);
            --gauge-fill: var(--gauge-fill-light);
            --button-bg: var(--button-bg-light);
            --button-active-bg: var(--button-active-bg-light);
            --button-active-text: var(--button-active-text-light);
        }

        [data-theme="dark"] {
            --bg-color: var(--bg-color-dark);
            --card-bg: var(--card-bg-dark);
            --text-color: var(--text-color-dark);
            --primary-color: var(--primary-color-dark);
            --secondary-color: var(--secondary-color-dark);
            --accent-color: var(--accent-color-dark);
            --success-color: var(--success-color-dark);
            --danger-color: var(--danger-color-dark);
            --primary-gradient: var(--primary-gradient-dark);
            --secondary-gradient: var(--secondary-gradient-dark);
            --accent-gradient: var(--accent-gradient-dark);
            --border-color: var(--border-color-dark);
            --shadow: var(--shadow-dark);
            --gauge-bg: var(--gauge-bg-dark);
            --gauge-fill: var(--gauge-fill-dark);
            --button-bg: var(--button-bg-dark);
            --button-active-bg: var(--button-active-bg-dark);
            --button-active-text: var(--button-active-text-dark);
        }

        * { 
            box-sizing: border-box; 
            margin: 0; 
            padding: 0;
            -webkit-tap-highlight-color: transparent; /* Remove tap highlight on mobile */
        }

        html, body {
            height: 100%;
            width: 100%;
            overflow-x: hidden;
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background-color: var(--bg-color);
            color: var(--text-color);
            line-height: 1.5;
            transition: background-color 0.4s ease, color 0.4s ease;
            padding: env(safe-area-inset-top, 20px) env(safe-area-inset-right, 20px) 
                    env(safe-area-inset-bottom, 20px) env(safe-area-inset-left, 20px);
            /* Add a subtle gradient background */
            background-image: linear-gradient(to bottom right, 
                                             var(--bg-color), 
                                             color-mix(in srgb, var(--bg-color) 90%, var(--primary-color)));
            min-height: 100vh; /* Full height */
        }

        .container { 
            max-width: 1200px; 
            margin: 0 auto;
            padding: 0 15px;
        }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
            padding: 15px 0;
            border-bottom: 1px solid var(--border-color);
        }
        
        header h1 { 
            font-size: 2em; 
            font-weight: 700;
            background: var(--primary-gradient);
            -webkit-background-clip: text;
            background-clip: text;
            color: transparent;
            position: relative;
        }

        .theme-switcher {
            background: var(--button-bg);
            border: 1px solid var(--border-color);
            color: var(--text-color);
            padding: 10px 18px;
            border-radius: 20px;
            cursor: pointer;
            font-size: 0.9em;
            transition: all 0.3s;
            backdrop-filter: blur(5px);
            -webkit-backdrop-filter: blur(5px);
        }
        
        .theme-switcher:hover {
            transform: translateY(-2px);
            box-shadow: var(--shadow);
            background: var(--primary-gradient);
            color: var(--button-active-text);
            border-color: transparent;
        }

        .grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); 
            gap: 20px;
            margin-bottom: 80px; /* Space for status indicator */
        }

        .card {
            background-color: var(--card-bg);
            border-radius: 16px;
            padding: 20px;
            box-shadow: var(--shadow);
            border: 1px solid var(--border-color);
            transition: all 0.3s ease;
            display: flex;
            flex-direction: column;
            overflow: hidden;
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
        }
        
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 15px 30px -10px rgba(0, 0, 0, 0.15);
        }

        .card h2 {
            font-size: 1.15em;
            margin-bottom: 15px;
            color: var(--primary-color);
            display: flex;
            align-items: center;
            font-weight: 600;
        }
        
        .card h2 svg {
            margin-right: 10px;
            width: 22px;
            height: 22px;
            fill: currentColor;
            opacity: 0.9;
        }

        /* Brand new gauge implementation - simplified and reliable */
        .gauge-container {
            width: 160px;
            height: 160px;
            margin: 10px auto 15px auto;
            position: relative;
        }
        
        /* Simple circular background */
        .gauge-bg {
            width: 100%;
            height: 100%;
            border-radius: 50%;
            background-color: var(--gauge-bg);
            position: absolute;
            top: 0;
            left: 0;
            box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.15);
        }
        
        /* Circular progress bar - created using conic gradient */
        .gauge-progress {
            width: 100%;
            height: 100%;
            border-radius: 50%;
            position: absolute;
            top: 0;
            left: 0;
            /* Will be set by JS using --percentage CSS variable */
            background: conic-gradient(var(--gauge-fill) var(--percentage), transparent var(--percentage));
            /* Start at 12 o'clock position */
            transform: rotate(-90deg);
            transform-origin: center;
            transition: background 0.8s;
        }
        
        /* Central circle with value */
        .gauge-center {
            position: absolute;
            width: 70%;
            height: 70%;
            background: var(--card-bg);
            border-radius: 50%;
            top: 15%;
            left: 15%;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        
        /* Gauge value text */
        .gauge-value {
            font-size: 2.2em;
            font-weight: 700;
            background: var(--secondary-gradient);
            -webkit-background-clip: text;
            background-clip: text;
            color: transparent;
            line-height: 1;
        }
        
        .gauge-unit {
            font-size: 0.9em;
            color: var(--text-color);
            opacity: 0.8;
            margin-top: 5px;
        }
        
        /* Gauge tick marks */
        .gauge-tick {
            position: absolute;
            width: 3px;
            height: 8px;
            background-color: var(--text-color);
            opacity: 0.3;
            top: 0;
            left: 50%;
            transform-origin: 0% 80px;
        }
        
        .gauge-tick-large {
            height: 12px;
            width: 3px;
            opacity: 0.5;
        }
        
        /* Alert indicator for critical values */
        @keyframes pulse-alert {
            0% { transform: scale(0.95); opacity: 0.9; }
            50% { transform: scale(1.05); opacity: 1; }
            100% { transform: scale(0.95); opacity: 0.9; }
        }
        
        .gauge-alert {
            animation: pulse-alert 1s infinite ease-in-out;
            box-shadow: 0 0 10px rgba(239, 68, 68, 0.5);
        }
        
        /* Icon Fixes */
        .icon-container {
            display: flex;
            align-items: center;
            justify-content: center;
            width: 60px;
            height: 60px;
            margin-bottom: 15px;
        }
        
        .icon-container svg {
            width: 100%;
            height: 100%;
        }
        
        .card svg {
            display: block;
            max-width: 100%;
            max-height: 100%;
        }

        /* Ensure proper icon coloring */
        .card h2 svg {
            fill: currentColor;
        }

        /* No need for the gauge-dot element anymore */

        /* Specific styles for non-gauge sensors (rain) */
        .sensor-simple {
            text-align: center;
            margin: 20px auto;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100%;
        }
        
        .sensor-simple .icon-container {
            width: 60px;
            height: 60px;
            margin-bottom: 12px;
            animation: float 3s ease-in-out infinite;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        
        .sensor-simple svg {
            width: 100%;
            height: 100%;
        }
        
        .sensor-simple .icon-raining {
            fill: #60a5fa; /* Blue for rain */
            filter: drop-shadow(0 0 5px rgba(96, 165, 250, 0.6));
        }
        
        .sensor-simple .icon-clear {
            fill: #fbbf24; /* Yellow for clear */
            filter: drop-shadow(0 0 8px rgba(251, 191, 36, 0.6));
        }
        
        @keyframes float {
            0% { transform: translateY(0px); }
            50% { transform: translateY(-5px); }
            100% { transform: translateY(0px); }
        }
        
        .sensor-simple .sensor-value {
            font-size: 2.2em;
            font-weight: 700;
            margin-bottom: 5px;
            background: var(--secondary-gradient);
            -webkit-background-clip: text;
            background-clip: text;
            color: transparent;
        }
        
        .sensor-simple .sensor-unit {
            font-size: 0.9em;
            opacity: 0.8;
        }

        /* Button Group Controls */
        .control-buttons {
            display: flex;
            width: 100%;
            margin-top: auto;
            border-radius: 12px;
            overflow: hidden;
            background: var(--button-bg);
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.08);
        }
        
        .control-buttons button {
            flex-grow: 1;
            padding: 14px 8px;
            background: transparent;
            color: var(--text-color);
            border: none;
            font-size: 0.95em;
            font-weight: 500;
            cursor: pointer;
            position: relative;
            transition: all 0.3s ease;
            z-index: 1;
        }
        
        .control-buttons button::after {
            content: '';
            position: absolute;
            top: 0;
            right: 0;
            width: 1px;
            height: 100%;
            background-color: var(--border-color);
            z-index: 2;
        }
        
        .control-buttons button:last-child::after {
            display: none;
        }
        
        .control-buttons button:hover {
            background-color: rgba(0, 0, 0, 0.05);
        }
        
        [data-theme="dark"] .control-buttons button:hover {
            background-color: rgba(255, 255, 255, 0.05);
        }
        
        .control-buttons button.active {
            background: var(--button-active-bg);
            color: var(--button-active-text);
            font-weight: 600;
            box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.1);
            transform: scale(1.02);
            z-index: 2;
        }
        
        .control-status-badge {
            display: inline-block;
            padding: 4px 10px;
            border-radius: 12px;
            font-size: 0.8em;
            font-weight: 600;
            margin-bottom: 15px;
            background: var(--gauge-bg);
            color: var(--text-color);
            text-align: center;
            align-self: center;
        }
        
        .control-status-badge.on {
            background: var(--success-color);
            color: white;
        }
        
        .control-status-badge.off {
            background: var(--gauge-bg);
        }
        
        .control-status-badge.auto {
            background: var(--secondary-gradient);
            color: white;
        }

        .status-indicator {
            position: fixed;
            bottom: 20px;
            left: 50%;
            transform: translateX(-50%);
            padding: 8px 15px;
            border-radius: 20px;
            font-size: 0.9em;
            transition: all 0.3s ease;
            z-index: 1000;
            box-shadow: 0 3px 8px rgba(0, 0, 0, 0.15);
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            display: flex;
            align-items: center;
        }
        
        .status-indicator::before {
            content: '';
            display: inline-block;
            width: 10px;
            height: 10px;
            border-radius: 50%;
            margin-right: 8px;
            animation: pulse 1.5s infinite;
        }
        
        .status-indicator.connecting {
            background-color: rgba(251, 191, 36, 0.8);
            color: #000;
        }
        
        .status-indicator.connecting::before {
            background-color: #fbbf24;
        }
        
        .status-indicator.connected {
            background-color: rgba(34, 197, 94, 0.8);
            color: white;
        }
        
        .status-indicator.connected::before {
            background-color: white;
        }
        
        .status-indicator.disconnected {
            background-color: rgba(239, 68, 68, 0.8);
            color: white;
        }
        
        .status-indicator.disconnected::before {
            background-color: white;
        }
        
        @keyframes pulse {
            0% { opacity: 0.4; transform: scale(1); }
            50% { opacity: 1; transform: scale(1.1); }
            100% { opacity: 0.4; transform: scale(1); }
        }

        /* Media queries for better mobile experience */
        @media (max-width: 480px) {
            .grid {
                grid-template-columns: 1fr;
                gap: 15px;
            }
            
            .gauge-container {
                width: 130px;
                height: 130px;
            }
            
            .gauge-value {
                font-size: 1.8em;
            }
            
            header h1 {
                font-size: 1.7em;
            }
            
            .theme-switcher {
                padding: 8px 12px;
                font-size: 0.8em;
            }
        }

        /* Animations for page load */
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        .card {
            animation: fadeIn 0.5s ease-out forwards;
            opacity: 0;
        }

        .card:nth-child(1) { animation-delay: 0.1s; }
        .card:nth-child(2) { animation-delay: 0.2s; }
        .card:nth-child(3) { animation-delay: 0.3s; }
        .card:nth-child(4) { animation-delay: 0.4s; }
        .card:nth-child(5) { animation-delay: 0.5s; }
        .card:nth-child(6) { animation-delay: 0.6s; }
        .card:nth-child(7) { animation-delay: 0.7s; }
        .card:nth-child(8) { animation-delay: 0.8s; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>Plant Care</h1>
            <button id="theme-toggle" class="theme-switcher">Toggle Theme</button>
        </header>

        <div class="grid">
            <!-- Simplified Sensor Cards with Gauges -->
            <div class="card">
                <h2>
                    <svg viewBox="0 0 24 24"><path d="M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zm0 8c-1.65 0-3-1.35-3-3s1.35-3 3-3 3 1.35 3 3-1.35 3-3 3zM3.55 19.09l1.41 1.41 1.79-1.8-1.41-1.41-1.79 1.8zM11 1h2v3h-2zm8.49 4.95l1.41-1.41 1.8 1.79-1.42 1.42-1.79-1.8zM19.09 19.09l1.41 1.41-1.79 1.8-1.41-1.41 1.79-1.8zM1 11h3v2H1zm18 0h3v2h-3zm-8 8h2v3h-2zm-4.54-3.46l-1.8 1.79 1.42 1.42 1.79-1.8-1.41-1.41z"/></svg>
                    Light Level
                </h2>
                <div class="gauge-container" id="light-gauge">
                    <div class="gauge-bg"></div>
                    <div class="gauge-progress" id="light-progress"></div>
                    <div class="gauge-center">
                        <div class="gauge-value" id="light-value">--</div>
                        <div class="gauge-unit">%</div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h2>
                    <svg viewBox="0 0 24 24"><path d="M12 2C8.14 2 5 5.14 5 9c0 3.07 2.03 6.17 5.28 8.44.6.42 1.44.42 2.04 0C16.97 15.17 19 12.07 19 9c0-3.86-3.14-7-7-7zm0 11.5c-1.38 0-2.5-1.12-2.5-2.5s1.12-2.5 2.5-2.5 2.5 1.12 2.5 2.5-1.12 2.5-2.5 2.5z"/></svg>
                    Soil Moisture
                </h2>
                <div class="gauge-container" id="soil-gauge">
                    <div class="gauge-bg"></div>
                    <div class="gauge-progress" id="soil-progress"></div>
                    <div class="gauge-center">
                        <div class="gauge-value" id="soil-value">--</div>
                        <div class="gauge-unit">%</div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h2>
                    <svg viewBox="0 0 24 24"><path d="M15 13V5c0-1.66-1.34-3-3-3S9 3.34 9 5v8c-1.21.91-2 2.37-2 4 0 2.76 2.24 5 5 5s5-2.24 5-5c0-1.63-.79-3.09-2-4zm-3-7c.55 0 1 .45 1 1v6.27c-.63-.17-1.29-.27-2-.27s-1.37.1-2 .27V7c0-.55.45-1 1-1zm0 14c-1.65 0-3-1.35-3-3s1.35-3 3-3 3 1.35 3 3-1.35 3-3 3z"/></svg>
                    Temperature
                </h2>
                <div class="gauge-container" id="temp-gauge">
                    <div class="gauge-bg"></div>
                    <div class="gauge-progress" id="temp-progress"></div>
                    <div class="gauge-center">
                        <div class="gauge-value" id="temp-value">--</div>
                        <div class="gauge-unit">°C</div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h2>
                    <svg viewBox="0 0 24 24"><path d="M19.35 12.04C18.67 6.59 15.64 1 12 1 9.11 1 6.6 3.64 5.35 6.04 2.35 6.36 0 8.91 0 12c0 3.31 2.69 6 6 6h13c2.76 0 5-2.24 5-5 0-2.64-2.05-4.78-4.65-4.96zM19 16H6c-2.21 0-4-1.79-4-4 0-2.05 1.53-3.76 3.56-3.97L6.63 8l.5-.95C8.08 4.14 9.94 3 12 3c2.62 0 4.88 1.86 5.39 4.43l.3 1.5 1.53.11c1.55.1 2.78 1.41 2.78 2.96 0 1.65-1.35 3-3 3z"/><path d="M14.09 16.15l-1.97-1.97-1.41 1.41 3.38 3.38 7.09-7.09-1.41-1.41z"/></svg>
                    Humidity
                </h2>
                <div class="gauge-container" id="humidity-gauge">
                    <div class="gauge-bg"></div>
                    <div class="gauge-progress" id="humidity-progress"></div>
                    <div class="gauge-center">
                        <div class="gauge-value" id="humidity-value">--</div>
                        <div class="gauge-unit">%</div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h2>
                    <svg viewBox="0 0 24 24"><path d="M17.66 7.93L12 2.27 6.34 7.93c-3.12 3.12-3.12 8.19 0 11.31C7.9 20.8 9.95 21.58 12 21.58c2.05 0 4.1-.78 5.66-2.34 3.12-3.12 3.12-8.19 0-11.31zM12 19.59c-1.6 0-3.11-.62-4.24-1.76C6.62 16.69 6 15.19 6 13.59s.62-3.11 1.76-4.24L12 5.1v14.49z"/></svg>
                    Rain Sensor
                </h2>
                <div class="sensor-simple" id="rain-indicator">
                    <div class="icon-container">
                        <!-- Icon will be dynamically inserted -->
                    </div>
                    <div class="sensor-value" id="rain-text">--</div>
                    <div class="sensor-unit" id="rain-subtext"></div>
                </div>
            </div>

            <!-- Control Cards with Buttons (unchanged) -->
            <div class="card controls">
                <h2>
                    <svg class="icon-power" viewBox="0 0 24 24"><path d="M16.01 7L10 3v4H4v6h6v4l6.01-4H18V7h-1.99z"/></svg>
                    Pump Control
                </h2>
                <div class="control-status-badge" id="pump-badge">Status: --</div>
                <div class="control-buttons" id="pump-controls">
                    <button data-action="off">Off</button>
                    <button data-action="on">On</button>
                    <button data-action="auto">Auto</button>
                </div>
            </div>
            
            <div class="card controls">
                <h2>
                    <svg class="icon-sun" viewBox="0 0 24 24"><path d="M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zm0 8c-1.65 0-3-1.35-3-3s1.35-3 3-3 3 1.35 3 3-1.12 2.5-2.5 2.5zM3.55 19.09l1.41 1.41 1.79-1.8-1.41-1.41-1.79 1.8zM11 1h2v3h-2zm8.49 4.95l1.41-1.41 1.8 1.79-1.42 1.42-1.79-1.8zM19.09 19.09l1.41 1.41-1.79 1.8-1.41-1.41 1.79-1.8zM1 11h3v2H1zm18 0h3v2h-3zm-8 8h2v3h-2zm-4.54-3.46l-1.8 1.79 1.42 1.42 1.79-1.8-1.41-1.41z"/></svg>
                    Light Control
                </h2>
                <div class="control-status-badge" id="light-badge">Status: --</div>
                <div class="control-buttons" id="light-controls">
                    <button data-action="off">Off</button>
                    <button data-action="on">On</button>
                    <button data-action="auto">Auto</button>
                </div>
            </div>
            
            <div class="card controls">
                <h2>
                    <svg class="icon-wind" viewBox="0 0 24 24"><path d="M19.71 10.29l-2.83-2.83c-.39-.39-1.02-.39-1.41 0L14 9.17V4c0-.55-.45-1-1-1s-1 .45-1 1v5.17l-1.46-1.46c-.39-.39-1.02-.39-1.41 0l-2.83 2.83c-.39.39-.39 1.02 0 1.41L10.59 16H6c-.55 0-1 .45-1 1s.45 1 1 1h4.59l-4.7 4.7c-.39.39-.39 1.02 0 1.41.2.2.45.29.71.29s.51-.1.71-.29l12-12c.38-.39.38-1.03 0-1.42z"/></svg>
                    Fan Control
                </h2>
                <div class="control-status-badge" id="fan-badge">Status: --</div>
                <div class="control-buttons" id="fan-controls">
                    <button data-action="off">Off</button>
                    <button data-action="on">On</button>
                    <button data-action="auto">Auto</button>
                </div>
            </div>
        </div>
    </div>

    <div id="status-indicator" class="status-indicator connecting">Connecting...</div>

    <script>
        // Variables for connection status and data fetching
        const statusIndicator = document.getElementById('status-indicator');
        let isConnected = false;
        let dataFetchInterval = null;
        let fetchTimeout = null;
        
        // For easy mapping between values
        const modeToAction = { 0: 'off', 1: 'on', 2: 'auto' };
        const actionToMode = { 'off': 0, 'on': 1, 'auto': 2 };

        // --- Completely revised Gauge Update Function ---
        // First, let's create the tick marks when the page loads
        function createTicks() {
            const gaugeIds = ['light-ticks', 'soil-ticks', 'temp-ticks', 'humidity-ticks'];
            
            gaugeIds.forEach(tickContainerId => {
                const container = document.getElementById(tickContainerId);
                if (!container) return;
                
                // Clear any existing ticks
                container.innerHTML = '';
                
                // Create 11 ticks (0%, 10%, 20%, ... 100%)
                for (let i = 0; i <= 10; i++) {
                    const tick = document.createElement('div');
                    tick.className = i % 5 === 0 ? 'gauge-tick gauge-tick-large' : 'gauge-tick';
                    
                    // Position the tick
                    const angle = i * 18; // 0 to 180 degrees in steps of 18
                    tick.style.transform = `rotate(${angle}deg)`;
                    
                    container.appendChild(tick);
                }
            });
        }

        // Simplified gauge update function using CSS variables
        function updateGauge(id, value, max = 100) {
            // Find the progress element
            const progressElement = document.getElementById(`${id}-progress`);
            const valueElement = document.getElementById(`${id}-value`);
            
            if (!progressElement || !valueElement) {
                console.error(`Missing elements for gauge ${id}`);
                return;
            }
            
            // Constrain the value
            const clampedValue = Math.max(0, Math.min(value, max));
            
            // Calculate percentage for the conic gradient (0-360 degrees)
            const percentage = Math.round((clampedValue / max) * 100);
            const degrees = (percentage / 100) * 360;
            
            // Update the CSS variable for the conic gradient
            progressElement.style.setProperty('--percentage', `${degrees}deg`);
            
            // Update the text display with proper formatting
            valueElement.textContent = value.toFixed(id.includes('temp') ? 1 : 0);
            
            // Customize colors based on value range
            let fillColor;
            let alertState = false;
            
            // Customize based on gauge type and value
            if (id === 'light-progress' || id === 'light') {
                if (clampedValue < 20) {
                    fillColor = '#3b82f6'; // Blue for low light
                } else if (clampedValue > 80) {
                    fillColor = '#f97316'; // Orange for high light
                } else {
                    fillColor = '#22c55e'; // Green for normal
                }
            } 
            else if (id === 'soil-progress' || id === 'soil') {
                if (clampedValue < 30) {
                    fillColor = '#ef4444'; // Red for dry soil
                    alertState = true;
                } else if (clampedValue > 80) {
                    fillColor = '#3b82f6'; // Blue for wet soil
                } else {
                    fillColor = '#22c55e'; // Green for normal
                }
            } 
            else if (id === 'temp-progress' || id === 'temp') {
                if (clampedValue < 15) {
                    fillColor = '#3b82f6'; // Blue for cold
                } else if (clampedValue > 30) {
                    fillColor = '#ef4444'; // Red for hot
                    alertState = true;
                } else {
                    fillColor = '#22c55e'; // Green for normal
                }
            } 
            else if (id === 'humidity-progress' || id === 'humidity') {
                if (clampedValue < 30) {
                    fillColor = '#f97316'; // Orange for dry
                } else if (clampedValue > 80) {
                    fillColor = '#3b82f6'; // Blue for humid
                } else {
                    fillColor = '#22c55e'; // Green for normal
                }
            }
            
            // Apply the color to the progress bar
            if (fillColor) {
                progressElement.style.background = `conic-gradient(${fillColor} var(--percentage), transparent var(--percentage))`;
            }
            
            // Add or remove alert class
            progressElement.classList.toggle('gauge-alert', alertState);
        }

        // --- Rain Indicator Update ---
        const rainIconSVG = `<svg viewBox="0 0 24 24"><path d="M17.66 13.69c-1.95 2.12-4.12 4.35-4.36 7.86-.3.54-.88.89-1.52.79-.58-.09-1.05-.56-1.15-1.15-.42-3.21-2.46-5.49-4.3-7.54C4.67 11.78 4 10.19 4 8.5 4 5.46 6.46 3 9.5 3S15 5.46 15 8.5c0 1.44-.53 2.76-1.41 3.77.92 1.05 2.02 2.29 2.66 3.73.19.44.74.62 1.16.43.43-.19.61-.74.42-1.17-.61-1.38-1.67-2.57-2.57-3.59.88-1.12 1.41-2.54 1.41-4.08C16.5 4.63 14.14 2 11 2S5.5 4.63 5.5 8c0 2.47 1.49 4.7 3.29 6.61 2.12 2.31 4.31 4.7 4.31 8.39 0 1.11.9 2 2 2s2-.89 2-2c.01-2.83 2.16-5.06 4.06-7.17.4-.44.37-1.13-.08-1.54-.45-.42-1.13-.39-1.55.05z"/></svg>`;
        const clearIconSVG = `<svg viewBox="0 0 24 24"><path d="M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zm0 8c-1.65 0-3-1.35-3-3s1.35-3 3-3 3 1.35 3 3-1.35 3-3 3zM3.55 19.09l1.41 1.41 1.79-1.8-1.41-1.41-1.79 1.8zM11 1h2v3h-2zm8.49 4.95l1.41-1.41 1.8 1.79-1.42 1.42-1.79-1.8zM19.09 19.09l1.41 1.41-1.79 1.8-1.41-1.41 1.79-1.8zM1 11h3v2H1zm18 0h3v2h-3zm-8 8h2v3h-2zm-4.54-3.46l-1.8 1.79 1.42 1.42 1.79-1.8-1.41-1.41z"/></svg>`;

        function updateRainIndicator(isRaining) {
            const iconContainer = document.querySelector('#rain-indicator .icon-container');
            const textEl = document.getElementById('rain-text');
            const subTextEl = document.getElementById('rain-subtext');
            
            if (!iconContainer || !textEl || !subTextEl) return;
            
            // Update icon
            iconContainer.innerHTML = isRaining ? rainIconSVG : clearIconSVG;
            const iconSvg = iconContainer.querySelector('svg');
            if (iconSvg) {
                iconSvg.style.fill = isRaining ? '#60a5fa' : '#fbbf24'; // Blue for rain, yellow for clear
                iconSvg.style.filter = isRaining 
                    ? 'drop-shadow(0 0 5px rgba(96, 165, 250, 0.6))' 
                    : 'drop-shadow(0 0 8px rgba(251, 191, 36, 0.6))';
            }
            
            // Update text
            textEl.textContent = isRaining ? 'Raining' : 'Clear';
            subTextEl.textContent = isRaining ? 'Rain detected' : 'No rain detected';
        }

        // --- Control Button Update ---
        function updateControlButtons(controlId, activeAction) {
            const buttons = document.querySelectorAll(`#${controlId} button`);
            buttons.forEach(button => {
                button.classList.toggle('active', button.dataset.action === activeAction);
            });
        }

        // --- Status Badge Update ---
        function updateStatusBadge(id, isActive, mode) {
            const badge = document.getElementById(`${id}-badge`);
            if (!badge) return;
            
            const action = modeToAction[mode] || 'off';
            const displayText = `Status: ${isActive ? 'ON' : 'OFF'}`;
            
            badge.textContent = displayText;
            badge.className = 'control-status-badge';
            badge.classList.add(action);
        }

        // --- Main UI Update Function ---
        function updateUI(data) {
            // Update gauge displays
            updateGauge('light', data.light !== undefined ? data.light : 0);
            updateGauge('soil', data.soil !== undefined ? data.soil : 0);
            updateGauge('temp', data.temperature !== undefined ? data.temperature : 0, 50);
            updateGauge('humidity', data.humidity !== undefined ? data.humidity : 0);

            // Update rain indicator
            updateRainIndicator(data.rain !== undefined ? data.rain === 1 : false);

            // Update control elements
            const pumpAction = modeToAction[data.pumpMode] || 'auto';
            updateStatusBadge('pump', data.pumpActive, data.pumpMode);
            updateControlButtons('pump-controls', pumpAction);

            const lightAction = modeToAction[data.lightMode] || 'off';
            updateStatusBadge('light', data.lightMode === 1, data.lightMode);
            updateControlButtons('light-controls', lightAction);

            const fanAction = modeToAction[data.fanMode] || 'off';
            updateStatusBadge('fan', data.fanActive, data.fanMode);
            updateControlButtons('fan-controls', fanAction);
        }

        // --- Data Fetching ---
        async function fetchData() {
            if (fetchTimeout) clearTimeout(fetchTimeout);
            
            const controller = new AbortController();
            fetchTimeout = setTimeout(() => { 
                controller.abort(); 
                console.error('Fetch timed out'); 
                if (isConnected) setConnectionStatus(false); 
            }, 5000);

            try {
                const response = await fetch('/api/data', { signal: controller.signal });
                clearTimeout(fetchTimeout);
                
                if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
                
                const data = await response.json();
                updateUI(data);
                
                if (!isConnected) setConnectionStatus(true);
            } catch (error) {
                if (error.name !== 'AbortError') {
                    console.error('Error fetching data:', error);
                    if (isConnected) setConnectionStatus(false);
                }
            }
        }

        // --- Control Commands ---
        async function sendControlCommand(device, action) {
            if (!actionToMode.hasOwnProperty(action)) { 
                console.error(`Invalid action: ${action}`); 
                return; 
            }
            
            // Provide immediate visual feedback
            if (device === 'pump') {
                updateControlButtons('pump-controls', action);
            } else if (device === 'light') {
                updateControlButtons('light-controls', action);
            } else if (device === 'fan') {
                updateControlButtons('fan-controls', action);
            }
            
            try {
                const response = await fetch('/api/control', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `device=${device}&action=${action}`
                });
                
                if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
                
                console.log(`Command sent: ${device} action ${action}`);
                setTimeout(fetchData, 250); // Fetch update after short delay
            } catch (error) {
                console.error('Error sending command:', error);
                setConnectionStatus(false);
            }
        }

        // --- Connection Status Management ---
        function setConnectionStatus(connected) {
            if (isConnected === connected) return;
            
            isConnected = connected;
            
            if (connected) {
                statusIndicator.textContent = 'Connected';
                statusIndicator.className = 'status-indicator connected';
                
                if (!dataFetchInterval) {
                    fetchData(); // Immediate fetch
                    dataFetchInterval = setInterval(fetchData, 3000);
                }
            } else {
                statusIndicator.textContent = 'Disconnected';
                statusIndicator.className = 'status-indicator disconnected';
                
                if (dataFetchInterval) {
                    clearInterval(dataFetchInterval);
                    dataFetchInterval = null;
                }
            }
        }

        // --- Theme Switching ---
        const themeToggle = document.getElementById('theme-toggle');
        const currentTheme = localStorage.getItem('theme') || 'light';
        document.documentElement.setAttribute('data-theme', currentTheme);
        themeToggle.textContent = currentTheme === 'dark' ? 'Light Mode' : 'Dark Mode';
        
        themeToggle.addEventListener('click', () => {
            let newTheme = document.documentElement.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
            document.documentElement.setAttribute('data-theme', newTheme);
            localStorage.setItem('theme', newTheme);
            themeToggle.textContent = newTheme === 'dark' ? 'Light Mode' : 'Dark Mode';
        });

        // --- Control Button Event Listeners ---
        function setupControlListeners(controlId, deviceName) {
            const controlDiv = document.getElementById(controlId);
            if (controlDiv) {
                controlDiv.addEventListener('click', (e) => {
                    if (e.target.tagName === 'BUTTON') {
                        const action = e.target.dataset.action;
                        if (action) {
                            sendControlCommand(deviceName, action);
                        }
                    }
                });
            }
        }
        
        setupControlListeners('pump-controls', 'pump');
        setupControlListeners('light-controls', 'light');
        setupControlListeners('fan-controls', 'fan');

        // --- Initial Setup & Screen Management ---
        // Fix for iOS Safari - Handle full screen and dismiss the address bar
        function fixIOSSafari() {
            // For iOS Safari only
            if (/iPad|iPhone|iPod/.test(navigator.userAgent) && !window.MSStream) {
                // Create dummy input to ensure we can focus something
                const dummyInput = document.createElement('input');
                dummyInput.setAttribute('type', 'text');
                dummyInput.style.position = 'absolute';
                dummyInput.style.opacity = 0;
                dummyInput.style.height = 0;
                dummyInput.style.fontSize = '16px'; // iOS won't zoom in if font size >= 16px
                
                document.body.appendChild(dummyInput);
                
                // On first touch, focus and blur to trigger full screen
                window.addEventListener('touchend', function onFirstTouch() {
                    setTimeout(function() {
                        dummyInput.focus();
                        setTimeout(function(){
                            dummyInput.blur();
                        }, 100);
                    }, 500);
                    
                    window.removeEventListener('touchend', onFirstTouch);
                }, false);
                
                // Scroll to hide Safari UI
                setTimeout(() => window.scrollTo(0, 1), 100);
                document.addEventListener('touchmove', () => setTimeout(() => window.scrollTo(0, 1), 100));
                window.addEventListener('orientationchange', () => setTimeout(() => window.scrollTo(0, 1), 100));
                window.addEventListener('resize', () => setTimeout(() => window.scrollTo(0, 1), 100));
            }
        }
        
        fixIOSSafari();

        // Start the application
        setConnectionStatus(false); // Initialize as disconnected
        fetchData();        // Initial data fetch attempt
    </script>
</body>
</html>
)rawliteral";

#endif // HTML_CONTENT_H