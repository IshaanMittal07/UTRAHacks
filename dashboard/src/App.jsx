/**
 * Arctic Analytics Dashboard
 *
 * A real-time robot telemetry and performance monitoring dashboard for UTRA Hacks 2026.
 * Visualizes sensor data from Arduino-based robots stored in MongoDB Atlas.
 *
 * Features:
 * - Real-time metrics display (duration, distance, motor speed)
 * - Interactive charts (area, bar, pie) using Recharts
 * - AI-powered voice explanations via ElevenLabs TTS
 * - Dark/light theme support with animated background
 *
 * @author Arctic Analytics Team
 * @version 1.0.0
 */

import { useState, useEffect, useMemo, useRef, useCallback } from 'react'
import {
  LineChart, Line, AreaChart, Area, XAxis, YAxis, CartesianGrid,
  Tooltip, ResponsiveContainer, PieChart, Pie, Cell, BarChart, Bar
} from 'recharts'
import {
  LayoutDashboard, Activity, Gauge, Moon,
  Clock, Radio, TrendingUp, Cpu, Volume2, VolumeX, Loader2
} from 'lucide-react'
import { textToSpeech, playAudioBlob, generateDashboardExplanation } from './elevenlabs'


/* =============================================================================
   BACKGROUND COMPONENTS
   Visual elements that create the arctic atmosphere
   ============================================================================= */

/**
 * MountainBackground Component
 *
 * Renders a layered SVG mountain range with snow-capped peaks.
 * Uses three layers of mountains with different opacities to create depth.
 * Colors adapt based on dark/light mode.
 *
 * @param {boolean} darkMode - Whether dark mode is enabled
 */
function MountainBackground({ darkMode }) {
  return (
    <div className="absolute inset-0 overflow-hidden pointer-events-none">
      <svg
        className="absolute bottom-0 w-full h-[60%]"
        viewBox="0 0 1440 400"
        preserveAspectRatio="xMidYMax slice"
        xmlns="http://www.w3.org/2000/svg"
      >
        {/* Background mountains - furthest layer, lowest opacity */}
        <path
          d="M0 400 L0 280 L120 180 L240 250 L360 140 L480 220 L600 100 L720 200 L840 80 L960 180 L1080 120 L1200 200 L1320 150 L1440 220 L1440 400 Z"
          fill={darkMode ? '#1e293b' : '#94a3b8'}
          opacity={darkMode ? 0.4 : 0.3}
        />
        {/* Middle mountains - medium layer */}
        <path
          d="M0 400 L0 320 L100 240 L200 300 L320 200 L440 280 L560 160 L680 260 L800 180 L920 280 L1040 200 L1160 300 L1280 220 L1400 280 L1440 260 L1440 400 Z"
          fill={darkMode ? '#334155' : '#64748b'}
          opacity={darkMode ? 0.6 : 0.4}
        />
        {/* Foreground mountains - closest layer, highest opacity */}
        <path
          d="M0 400 L0 340 L80 280 L180 340 L280 260 L400 320 L520 240 L640 320 L760 260 L880 340 L1000 280 L1120 340 L1240 290 L1360 350 L1440 310 L1440 400 Z"
          fill={darkMode ? '#475569' : '#475569'}
          opacity={darkMode ? 0.8 : 0.5}
        />
        {/* Snow caps - white triangles on the tallest peaks */}
        <g fill="white" opacity={darkMode ? 0.3 : 0.6}>
          <polygon points="360,140 340,160 380,160" />
          <polygon points="600,100 575,125 625,125" />
          <polygon points="840,80 810,110 870,110" />
          <polygon points="1080,120 1055,145 1105,145" />
        </g>
      </svg>
    </div>
  )
}

/**
 * Snowfall Component
 *
 * Creates an animated snowfall effect with randomized snowflakes.
 * Each snowflake has random position, size, opacity, and animation timing.
 * Uses CSS animations defined in index.css for the falling motion.
 *
 * @param {number} count - Number of snowflakes to render (default: 40)
 */
function Snowfall({ count = 40 }) {
  // Generate snowflake properties once using useMemo to prevent re-renders
  const snowflakes = useMemo(() => {
    return Array.from({ length: count }, (_, i) => ({
      id: i,
      left: Math.random() * 100,                    // Random horizontal position (0-100%)
      animationDuration: 10 + Math.random() * 20,   // Fall duration (10-30 seconds)
      animationDelay: Math.random() * 10,           // Staggered start (0-10 seconds)
      size: 3 + Math.random() * 5,                  // Flake size (3-8 pixels)
      opacity: 0.3 + Math.random() * 0.4            // Visibility (0.3-0.7)
    }))
  }, [count])

  return (
    <div className="absolute inset-0 pointer-events-none overflow-hidden z-0">
      {snowflakes.map(flake => (
        <div
          key={flake.id}
          className="snowflake"
          style={{
            left: `${flake.left}%`,
            animationDuration: `${flake.animationDuration}s`,
            animationDelay: `${flake.animationDelay}s`,
            width: `${flake.size}px`,
            height: `${flake.size}px`,
            opacity: flake.opacity
          }}
        />
      ))}
    </div>
  )
}


/* =============================================================================
   DATA UTILITIES
   Functions for generating and processing sensor data
   ============================================================================= */

/**
 * Generate Mock Sensor Data
 *
 * Creates sample data matching the bridge.py schema for testing/demo purposes.
 * Used as fallback when MongoDB connection is unavailable.
 *
 * Schema:
 * - t: timestamp in milliseconds
 * - r, g, b: RGB color values (0-100)
 * - color: detected color code (R/G/B/U)
 * - line: line sensor reading (W=white, B=black)
 * - dist: ultrasonic distance in cm
 * - ml, mr: left and right motor speeds in RPM
 *
 * @returns {Array} Array of 15 mock sensor readings
 */
function generateMockData() {
  const colors = ['R', 'G', 'B', 'U']  // Red, Green, Blue, Unknown
  const data = []

  for (let i = 0; i < 15; i++) {
    data.push({
      t: i * 1000,                                    // Timestamp: 0, 1000, 2000... ms
      r: 30 + Math.random() * 70,                     // Red value: 30-100
      g: 30 + Math.random() * 70,                     // Green value: 30-100
      b: 30 + Math.random() * 70,                     // Blue value: 30-100
      color: colors[Math.floor(Math.random() * 4)],   // Random color code
      line: Math.random() > 0.5 ? 'W' : 'B',          // 50/50 white or black
      dist: 15 + Math.random() * 50,                  // Distance: 15-65 cm
      ml: 150 + Math.random() * 70,                   // Left motor: 150-220 RPM
      mr: 150 + Math.random() * 70                    // Right motor: 150-220 RPM
    })
  }
  return data
}


/* =============================================================================
   UI COMPONENTS
   Reusable interface components
   ============================================================================= */

/**
 * Sidebar Component
 *
 * Main navigation sidebar with:
 * - App logo and branding
 * - Navigation menu
 * - Dark/light theme toggle
 * - Penguin mascot with TTS button
 * - Data source connection indicator
 *
 * @param {string} activeTab - Currently active navigation tab
 * @param {function} setActiveTab - Tab change handler
 * @param {boolean} darkMode - Dark mode state
 * @param {function} setDarkMode - Dark mode toggle handler
 * @param {string} dataSource - Current data source ('mongodb', 'mock', 'loading')
 * @param {function} onSpeak - TTS trigger handler
 * @param {boolean} isSpeaking - Whether audio is currently playing
 * @param {boolean} isLoadingAudio - Whether audio is being fetched
 */
function Sidebar({ activeTab, setActiveTab, darkMode, setDarkMode, dataSource, onSpeak, isSpeaking, isLoadingAudio }) {
  // Navigation items configuration
  const navItems = [
    { id: 'dashboard', label: 'Dashboard', icon: LayoutDashboard },
  ]

  return (
    <div className={`w-64 h-full flex flex-col ${darkMode ? 'bg-slate-900' : 'bg-slate-800'} p-6`}>

      {/* App Logo & Branding */}
      <div className="flex items-center gap-3 mb-10">
        <div className="w-10 h-10 flex items-center justify-center">
          <span className="text-2xl">❄️</span>
        </div>
        <div className="flex flex-col">
          <span className="text-white text-lg tracking-wide">Arctic Analytics</span>
          <span className="text-sky-400 text-xs tracking-wider">UTRA HACKS 2026</span>
        </div>
      </div>

      {/* Navigation Menu */}
      <nav className="flex-1 space-y-2">
        {navItems.map(item => (
          <button
            key={item.id}
            onClick={() => setActiveTab(item.id)}
            className={`w-full flex items-center gap-3 px-4 py-3 rounded-lg transition-all duration-200 ${
              activeTab === item.id
                ? 'bg-sky-500/20 text-sky-400'
                : 'text-slate-400 hover:text-white hover:bg-slate-700/50'
            }`}
          >
            <item.icon className="w-5 h-5" />
            <span className="text-sm">{item.label}</span>
          </button>
        ))}

        {/* Theme Toggle Switch */}
        <div className="flex items-center justify-between px-4 py-3 text-slate-400">
          <div className="flex items-center gap-3">
            <Moon className="w-5 h-5" />
            <span className="text-sm">{darkMode ? 'Dark' : 'Light'}</span>
          </div>
          <button
            onClick={() => setDarkMode(!darkMode)}
            className={`w-11 h-6 rounded-full transition-all duration-300 ${
              darkMode ? 'bg-sky-500' : 'bg-amber-400'
            } relative`}
          >
            <div className={`w-4 h-4 rounded-full bg-white absolute top-1 transition-all duration-300 ${
              darkMode ? 'left-6' : 'left-1'
            }`} />
          </button>
        </div>
      </nav>

      {/* Penguin Mascot Section - AI Voice Guide */}
      <div className="mt-4 mb-4">
        <div className="flex flex-col items-center">
          {/* Mascot Avatar - Placeholder for custom PNG */}
          <div className="w-24 h-24 rounded-2xl bg-gradient-to-br from-sky-400 to-sky-600 flex items-center justify-center mb-3 shadow-lg shadow-sky-500/20 border-2 border-sky-300/30">
            <span className="text-4xl">🐧</span>
          </div>

          {/* ElevenLabs TTS Trigger Button */}
          <button
            onClick={onSpeak}
            disabled={isLoadingAudio}
            className={`w-full flex items-center justify-center gap-2 px-4 py-2.5 rounded-lg transition-all duration-300 ${
              isSpeaking
                ? 'bg-rose-500/20 text-rose-400 hover:bg-rose-500/30'
                : 'bg-sky-500/20 text-sky-400 hover:bg-sky-500/30'
            } ${isLoadingAudio ? 'opacity-50 cursor-not-allowed' : 'cursor-pointer'}`}
          >
            {isLoadingAudio ? (
              <>
                <Loader2 className="w-4 h-4 animate-spin" />
                <span className="text-sm">Loading...</span>
              </>
            ) : isSpeaking ? (
              <>
                <VolumeX className="w-4 h-4" />
                <span className="text-sm">Stop</span>
              </>
            ) : (
              <>
                <Volume2 className="w-4 h-4" />
                <span className="text-sm">Explain Dashboard</span>
              </>
            )}
          </button>
        </div>
      </div>

      {/* Data Source Connection Indicator */}
      <div className={`mt-auto pt-4 border-t border-slate-700`}>
        <div className="flex items-center gap-2 px-4 py-2">
          <div className={`w-2 h-2 rounded-full ${
            dataSource === 'mongodb' ? 'bg-emerald-500 animate-pulse' :
            dataSource === 'mock' ? 'bg-amber-500' : 'bg-slate-500'
          }`} />
          <span className="text-xs text-slate-400">
            {dataSource === 'mongodb' ? 'MongoDB Connected' :
             dataSource === 'mock' ? 'Using Mock Data' : 'Connecting...'}
          </span>
        </div>
      </div>
    </div>
  )
}

/**
 * StatCard Component
 *
 * Displays a single metric with icon, value, unit, and optional sparkline chart.
 * Features hover animations and theme-aware styling.
 *
 * @param {string} title - Metric label
 * @param {string|number} value - Metric value
 * @param {string} unit - Unit of measurement
 * @param {Component} icon - Lucide icon component
 * @param {string} color - Color theme (sky, rose, emerald, amber, violet)
 * @param {boolean} darkMode - Dark mode state
 * @param {Array} sparklineData - Data points for mini chart [{v: number}]
 */
function StatCard({ title, value, unit, icon: Icon, color, darkMode, sparklineData }) {
  // Color class mapping for icon backgrounds
  const colorClasses = {
    sky: 'bg-sky-500',
    rose: 'bg-rose-500',
    emerald: 'bg-emerald-500',
    amber: 'bg-amber-500',
    violet: 'bg-violet-500'
  }

  return (
    <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-5 backdrop-blur-sm rounded-lg transition-all duration-300 hover:scale-105 hover:shadow-lg hover:shadow-sky-500/10 cursor-pointer`}>
      <div className="flex items-start justify-between mb-4">
        {/* Icon Container */}
        <div className={`w-12 h-12 ${colorClasses[color]} rounded-lg flex items-center justify-center transition-transform duration-300 hover:rotate-6`}>
          <Icon className="w-6 h-6 text-white" />
        </div>

        {/* Sparkline Mini Chart */}
        {sparklineData && sparklineData.length > 0 && (
          <div className="w-20 h-10">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={sparklineData}>
                <Line
                  type="monotone"
                  dataKey="v"
                  stroke={colorClasses[color].includes('emerald') ? '#22c55e' : colorClasses[color].includes('rose') ? '#ef4444' : '#0ea5e9'}
                  strokeWidth={2}
                  dot={false}
                />
              </LineChart>
            </ResponsiveContainer>
          </div>
        )}
      </div>

      {/* Metric Label */}
      <p className={`${darkMode ? 'text-slate-400' : 'text-slate-600'} text-sm mb-1`}>{title}</p>

      {/* Metric Value */}
      <p className={`${darkMode ? 'text-white' : 'text-slate-900'} text-2xl font-semibold`}>
        {value}<span className={`${darkMode ? 'text-slate-500' : 'text-slate-400'} text-base ml-1`}>{unit}</span>
      </p>
    </div>
  )
}


/* =============================================================================
   MAIN APPLICATION
   Root component managing state and rendering the dashboard
   ============================================================================= */

/**
 * App Component
 *
 * Main application component that:
 * - Manages global state (theme, data, audio)
 * - Fetches sensor data from MongoDB API
 * - Calculates derived metrics
 * - Renders the complete dashboard layout
 */
export default function App() {
  // === STATE MANAGEMENT ===

  // UI State
  const [darkMode, setDarkMode] = useState(true)           // Theme toggle
  const [activeTab, setActiveTab] = useState('dashboard')   // Navigation state

  // Data State
  const [runData, setRunData] = useState([])                // Sensor readings array
  const [dataSource, setDataSource] = useState('loading')   // 'mongodb', 'mock', 'loading'
  const [sessionInfo, setSessionInfo] = useState(null)      // Session metadata

  // Audio State for ElevenLabs TTS
  const [isSpeaking, setIsSpeaking] = useState(false)       // Playback active
  const [isLoadingAudio, setIsLoadingAudio] = useState(false) // API request pending
  const audioRef = useRef(null)                             // Audio element reference

  // === DATA FETCHING ===

  /**
   * Fetch latest session data from MongoDB API on mount.
   * Falls back to mock data if API is unavailable.
   */
  useEffect(() => {
    async function fetchData() {
      try {
        const res = await fetch('/api/latest')
        if (res.ok) {
          const data = await res.json()
          if (data && data.readings) {
            setRunData(data.readings)
            setSessionInfo({
              robot_id: data.robot_id,
              notes: data.notes,
              timestamp: data.timestamp
            })
            setDataSource('mongodb')
            return
          }
        }
      } catch {
        // API unavailable - fall through to mock data
      }
      // Fallback to generated mock data
      setRunData(generateMockData())
      setDataSource('mock')
    }
    fetchData()
  }, [])

  // === COMPUTED METRICS ===

  /**
   * Calculate derived metrics from raw sensor data.
   * Memoized to prevent recalculation on every render.
   */
  const metrics = useMemo(() => {
    if (!runData.length) return null

    // Extract data arrays
    const distances = runData.map(d => d.dist || 0)
    const motorL = runData.map(d => d.ml || 0)
    const motorR = runData.map(d => d.mr || 0)
    const duration = runData[runData.length - 1]?.t || 0

    // Count color detections
    const colorCounts = { R: 0, G: 0, B: 0, U: 0 }
    const lineCounts = { W: 0, B: 0 }

    runData.forEach(d => {
      if (d.color) colorCounts[d.color] = (colorCounts[d.color] || 0) + 1
      if (d.line) lineCounts[d.line] = (lineCounts[d.line] || 0) + 1
    })

    return {
      avgDistance: (distances.reduce((a, b) => a + b, 0) / distances.length).toFixed(1),
      minDistance: Math.min(...distances).toFixed(1),
      avgSpeed: ((motorL.reduce((a, b) => a + b, 0) + motorR.reduce((a, b) => a + b, 0)) / (2 * motorL.length)).toFixed(0),
      duration: (duration / 1000).toFixed(1),
      dataPoints: runData.length,
      colorCounts,
      lineCounts
    }
  }, [runData])

  // === EVENT HANDLERS ===

  /**
   * Handle TTS speak button click.
   * Toggles between playing and stopping audio.
   * Fetches audio from ElevenLabs API if not already playing.
   */
  const handleSpeak = useCallback(async () => {
    // Stop if already speaking
    if (isSpeaking && audioRef.current) {
      audioRef.current.pause()
      audioRef.current = null
      setIsSpeaking(false)
      return
    }

    // Check for API key in environment
    const apiKey = import.meta.env.VITE_ELEVENLABS_API_KEY
    if (!apiKey) {
      alert('Please set VITE_ELEVENLABS_API_KEY in your .env file to enable voice explanations.')
      return
    }

    setIsLoadingAudio(true)

    try {
      // Generate explanation and convert to speech
      const explanationText = generateDashboardExplanation(metrics, sessionInfo)
      const audioBlob = await textToSpeech(explanationText, apiKey)
      const audio = playAudioBlob(audioBlob)

      audioRef.current = audio
      setIsSpeaking(true)

      // Handle playback end
      audio.onended = () => {
        setIsSpeaking(false)
        audioRef.current = null
      }

      // Handle playback error
      audio.onerror = () => {
        setIsSpeaking(false)
        audioRef.current = null
      }
    } catch (error) {
      console.error('ElevenLabs TTS error:', error)
      alert(`Voice generation failed: ${error.message}`)
    } finally {
      setIsLoadingAudio(false)
    }
  }, [isSpeaking, metrics, sessionInfo])

  // === CHART DATA PREPARATION ===

  // Transform raw data for charts
  const distanceChartData = runData.map(d => ({
    time: (d.t / 1000).toFixed(1),
    distance: Math.round(d.dist || 0),
    motorL: Math.round(d.ml || 0),
    motorR: Math.round(d.mr || 0)
  }))

  // Sparkline data (last 7 readings)
  const distanceSparkline = runData.slice(-7).map(d => ({ v: d.dist || 0 }))
  const motorSparkline = runData.slice(-7).map(d => ({ v: ((d.ml || 0) + (d.mr || 0)) / 2 }))
  const durationSparkline = runData.slice(-7).map(d => ({ v: (d.t || 0) / 1000 }))

  // === RENDER ===

  return (
    <div className={`min-h-screen p-4 ${darkMode ? 'bg-slate-950' : 'bg-gradient-to-br from-sky-100 to-slate-200'}`}>
      <div className={`h-[calc(100vh-2rem)] ${darkMode ? 'bg-slate-900/50 border-slate-700/50' : 'bg-white/70 border-slate-300/50'} overflow-hidden flex backdrop-blur-sm border`}>

        {/* Left Sidebar */}
        <Sidebar
          activeTab={activeTab}
          setActiveTab={setActiveTab}
          darkMode={darkMode}
          setDarkMode={setDarkMode}
          dataSource={dataSource}
          onSpeak={handleSpeak}
          isSpeaking={isSpeaking}
          isLoadingAudio={isLoadingAudio}
        />

        {/* Main Content Area */}
        <div className="flex-1 flex flex-col overflow-hidden relative">
          {/* Background Elements */}
          <MountainBackground darkMode={darkMode} />
          <Snowfall count={30} />

          {/* Dashboard Content */}
          <div className="flex-1 overflow-auto px-6 py-6 relative z-10">
            <div className="grid grid-cols-12 gap-5">

              {/* Left Column - Stats & Charts (8 cols) */}
              <div className="col-span-8">

                {/* Stat Cards Row */}
                <div className="grid grid-cols-3 gap-4 mb-5">
                  {metrics && (
                    <>
                      <StatCard title="Duration" value={metrics.duration} unit="s" icon={Clock} color="sky" darkMode={darkMode} sparklineData={durationSparkline} />
                      <StatCard title="Avg Distance" value={metrics.avgDistance} unit="cm" icon={Radio} color="rose" darkMode={darkMode} sparklineData={distanceSparkline} />
                      <StatCard title="Motor Speed" value={metrics.avgSpeed} unit="RPM" icon={Gauge} color="emerald" darkMode={darkMode} sparklineData={motorSparkline} />
                    </>
                  )}
                </div>

                {/* Main Time-Series Chart */}
                <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-6 backdrop-blur-sm rounded-lg mb-5 transition-all duration-300 hover:shadow-lg hover:shadow-sky-500/10`}>
                  <div className="flex items-center justify-between mb-4">
                    <div>
                      <h3 className={`${darkMode ? 'text-white' : 'text-slate-800'} text-lg font-medium`}>Sensor Data Over Time</h3>
                      <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs mt-1`}>Distance (cm) & Motor Speed (RPM) from ultrasonic sensor</p>
                    </div>
                    <div className={`${darkMode ? 'bg-slate-700' : 'bg-slate-200'} rounded px-3 py-1.5 text-xs ${darkMode ? 'text-slate-300' : 'text-slate-600'}`}>
                      MongoDB Data
                    </div>
                  </div>

                  {/* Area Chart */}
                  <ResponsiveContainer width="100%" height={200}>
                    <AreaChart data={distanceChartData}>
                      <defs>
                        {/* Gradient fills for area charts */}
                        <linearGradient id="distGrad" x1="0" y1="0" x2="0" y2="1">
                          <stop offset="5%" stopColor="#22c55e" stopOpacity={0.3}/>
                          <stop offset="95%" stopColor="#22c55e" stopOpacity={0}/>
                        </linearGradient>
                        <linearGradient id="motorGrad" x1="0" y1="0" x2="0" y2="1">
                          <stop offset="5%" stopColor="#ef4444" stopOpacity={0.3}/>
                          <stop offset="95%" stopColor="#ef4444" stopOpacity={0}/>
                        </linearGradient>
                      </defs>
                      <CartesianGrid strokeDasharray="3 3" stroke={darkMode ? '#334155' : '#e2e8f0'} vertical={false} />
                      <XAxis
                        dataKey="time"
                        tick={{ fill: darkMode ? '#64748b' : '#475569', fontSize: 11 }}
                        axisLine={false}
                        tickLine={false}
                        label={{ value: 'Time (seconds)', position: 'bottom', fill: darkMode ? '#64748b' : '#475569', fontSize: 10 }}
                      />
                      <YAxis
                        tick={{ fill: darkMode ? '#64748b' : '#475569', fontSize: 11 }}
                        axisLine={false}
                        tickLine={false}
                      />
                      <Tooltip
                        contentStyle={{
                          backgroundColor: darkMode ? '#1e293b' : '#ffffff',
                          border: darkMode ? 'none' : '1px solid #e2e8f0',
                          borderRadius: '12px',
                          boxShadow: '0 4px 20px rgba(0,0,0,0.15)'
                        }}
                        labelStyle={{ color: darkMode ? '#94a3b8' : '#475569' }}
                        formatter={(value, name) => [
                          `${value} ${name === 'distance' ? 'cm' : 'RPM'}`,
                          name === 'distance' ? 'Distance' : 'Left Motor'
                        ]}
                        labelFormatter={(label) => `Time: ${label}s`}
                      />
                      <Area type="monotone" dataKey="distance" stroke="#22c55e" strokeWidth={2} fill="url(#distGrad)" name="distance" />
                      <Area type="monotone" dataKey="motorL" stroke="#ef4444" strokeWidth={2} fill="url(#motorGrad)" name="motorL" />
                    </AreaChart>
                  </ResponsiveContainer>

                  {/* Chart Legend */}
                  <div className="flex justify-center gap-6 mt-3">
                    <div className="flex items-center gap-2">
                      <div className="w-3 h-3 rounded bg-green-500" />
                      <span className={`${darkMode ? 'text-slate-400' : 'text-slate-600'} text-xs`}>Distance (cm)</span>
                    </div>
                    <div className="flex items-center gap-2">
                      <div className="w-3 h-3 rounded bg-red-500" />
                      <span className={`${darkMode ? 'text-slate-400' : 'text-slate-600'} text-xs`}>Left Motor (RPM)</span>
                    </div>
                  </div>
                </div>

                {/* Bottom Row - Motor Comparison & Session Info */}
                <div className="grid grid-cols-2 gap-5">

                  {/* Motor Comparison Bar Chart */}
                  <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-6 backdrop-blur-sm rounded-lg transition-all duration-300 hover:shadow-lg hover:shadow-sky-500/10`}>
                    <div className="mb-4">
                      <h3 className={`${darkMode ? 'text-white' : 'text-slate-800'} text-lg font-medium`}>Motor Comparison</h3>
                      <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs mt-1`}>Left vs Right motor speed (last 6 readings)</p>
                    </div>
                    <ResponsiveContainer width="100%" height={130}>
                      <BarChart data={distanceChartData.slice(-6)} barGap={2}>
                        <CartesianGrid strokeDasharray="3 3" stroke={darkMode ? '#334155' : '#e2e8f0'} vertical={false} />
                        <XAxis dataKey="time" tick={{ fill: darkMode ? '#64748b' : '#475569', fontSize: 10 }} axisLine={false} tickLine={false} />
                        <YAxis tick={{ fill: darkMode ? '#64748b' : '#475569', fontSize: 10 }} axisLine={false} tickLine={false} />
                        <Tooltip
                          contentStyle={{
                            backgroundColor: darkMode ? '#1e293b' : '#ffffff',
                            border: darkMode ? 'none' : '1px solid #e2e8f0',
                            borderRadius: '8px'
                          }}
                          formatter={(value) => [`${value} RPM`]}
                        />
                        <Bar dataKey="motorL" fill="#3b82f6" radius={[4, 4, 0, 0]} name="Left Motor" />
                        <Bar dataKey="motorR" fill="#8b5cf6" radius={[4, 4, 0, 0]} name="Right Motor" />
                      </BarChart>
                    </ResponsiveContainer>
                    <div className="flex justify-center gap-6 mt-3">
                      <div className="flex items-center gap-2">
                        <div className="w-3 h-3 rounded bg-blue-500" />
                        <span className={`${darkMode ? 'text-slate-400' : 'text-slate-600'} text-xs`}>Left Motor</span>
                      </div>
                      <div className="flex items-center gap-2">
                        <div className="w-3 h-3 rounded bg-violet-500" />
                        <span className={`${darkMode ? 'text-slate-400' : 'text-slate-600'} text-xs`}>Right Motor</span>
                      </div>
                    </div>
                  </div>

                  {/* Session Info Panel */}
                  <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-6 backdrop-blur-sm rounded-lg transition-all duration-300 hover:shadow-lg hover:shadow-sky-500/10`}>
                    <h3 className={`${darkMode ? 'text-white' : 'text-slate-800'} text-lg font-medium mb-4`}>Session Info</h3>
                    <div className="space-y-4">
                      {/* Robot ID */}
                      <div className="flex items-center gap-3">
                        <div className="w-10 h-10 rounded-lg bg-sky-500/20 flex items-center justify-center">
                          <Cpu className="w-5 h-5 text-sky-400" />
                        </div>
                        <div>
                          <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs`}>Robot ID</p>
                          <p className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium`}>{sessionInfo?.robot_id || 'N/A'}</p>
                        </div>
                      </div>
                      {/* Data Points */}
                      <div className="flex items-center gap-3">
                        <div className="w-10 h-10 rounded-lg bg-emerald-500/20 flex items-center justify-center">
                          <TrendingUp className="w-5 h-5 text-emerald-400" />
                        </div>
                        <div>
                          <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs`}>Data Points</p>
                          <p className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium`}>{metrics?.dataPoints || 0} readings</p>
                        </div>
                      </div>
                      {/* Session Notes */}
                      <div className="flex items-center gap-3">
                        <div className="w-10 h-10 rounded-lg bg-amber-500/20 flex items-center justify-center">
                          <Activity className="w-5 h-5 text-amber-400" />
                        </div>
                        <div>
                          <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs`}>Notes</p>
                          <p className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium text-sm truncate max-w-[180px]`}>{sessionInfo?.notes || 'No notes'}</p>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>

              </div>

              {/* Right Column - Pie Charts (4 cols) */}
              <div className="col-span-4 space-y-5">

                {/* Color Detection Pie Chart */}
                <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-6 backdrop-blur-sm rounded-lg transition-all duration-300 hover:shadow-lg hover:shadow-sky-500/10`}>
                  <h3 className={`${darkMode ? 'text-white' : 'text-slate-800'} text-lg font-medium mb-4`}>Color Detection</h3>
                  <ResponsiveContainer width="100%" height={220}>
                    <PieChart>
                      <Pie
                        data={[
                          { name: 'Red', value: metrics?.colorCounts?.R || 0 },
                          { name: 'Green', value: metrics?.colorCounts?.G || 0 },
                          { name: 'Blue', value: metrics?.colorCounts?.B || 0 },
                          { name: 'Unknown', value: metrics?.colorCounts?.U || 0 }
                        ]}
                        cx="50%"
                        cy="45%"
                        innerRadius={50}
                        outerRadius={75}
                        paddingAngle={3}
                        dataKey="value"
                        label={({ value, percent }) => value > 0 ? `${(percent * 100).toFixed(0)}%` : ''}
                        labelLine={false}
                      >
                        <Cell fill="#ef4444" />
                        <Cell fill="#22c55e" />
                        <Cell fill="#3b82f6" />
                        <Cell fill="#64748b" />
                      </Pie>
                      <Tooltip
                        contentStyle={{
                          backgroundColor: darkMode ? '#1e293b' : '#ffffff',
                          border: darkMode ? '1px solid #475569' : '1px solid #e2e8f0',
                          borderRadius: '8px',
                          padding: '8px 12px'
                        }}
                        itemStyle={{ color: darkMode ? '#ffffff' : '#1e293b' }}
                        labelStyle={{ color: darkMode ? '#94a3b8' : '#64748b' }}
                        formatter={(value, name) => [`${value} readings`, name]}
                      />
                    </PieChart>
                  </ResponsiveContainer>

                  {/* Color Legend Grid */}
                  <div className="grid grid-cols-2 gap-3 mt-3">
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-red-500" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>Red</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.colorCounts?.R || 0}</span>
                    </div>
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-green-500" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>Green</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.colorCounts?.G || 0}</span>
                    </div>
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-blue-500" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>Blue</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.colorCounts?.B || 0}</span>
                    </div>
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-slate-500" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>Unknown</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.colorCounts?.U || 0}</span>
                    </div>
                  </div>
                </div>

                {/* Line Following Pie Chart */}
                <div className={`${darkMode ? 'bg-slate-800/80' : 'bg-white/90'} p-6 backdrop-blur-sm rounded-lg transition-all duration-300 hover:shadow-lg hover:shadow-sky-500/10`}>
                  <div className="mb-4">
                    <h3 className={`${darkMode ? 'text-white' : 'text-slate-800'} text-lg font-medium`}>Line Following</h3>
                    <p className={`${darkMode ? 'text-slate-400' : 'text-slate-500'} text-xs mt-1`}>IR sensor line detection</p>
                  </div>
                  <ResponsiveContainer width="100%" height={200}>
                    <PieChart>
                      <Pie
                        data={[
                          { name: 'White', value: metrics?.lineCounts?.W || 0 },
                          { name: 'Black', value: metrics?.lineCounts?.B || 0 }
                        ]}
                        cx="50%"
                        cy="45%"
                        innerRadius={50}
                        outerRadius={75}
                        paddingAngle={3}
                        dataKey="value"
                        label={({ value, percent }) => value > 0 ? `${(percent * 100).toFixed(0)}%` : ''}
                        labelLine={false}
                      >
                        <Cell fill="#e2e8f0" />
                        <Cell fill="#334155" stroke="#64748b" strokeWidth={2} />
                      </Pie>
                      <Tooltip
                        contentStyle={{
                          backgroundColor: darkMode ? '#1e293b' : '#ffffff',
                          border: darkMode ? '1px solid #475569' : '1px solid #e2e8f0',
                          borderRadius: '8px',
                          padding: '8px 12px'
                        }}
                        itemStyle={{ color: darkMode ? '#ffffff' : '#1e293b' }}
                        labelStyle={{ color: darkMode ? '#94a3b8' : '#64748b' }}
                        formatter={(value, name) => [`${value} readings`, name]}
                      />
                    </PieChart>
                  </ResponsiveContainer>

                  {/* Line Legend */}
                  <div className="grid grid-cols-2 gap-3 mt-3">
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-slate-200 border border-slate-400" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>White</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.lineCounts?.W || 0}</span>
                    </div>
                    <div className={`flex items-center gap-2 ${darkMode ? 'bg-slate-700/50 hover:bg-slate-600/50' : 'bg-slate-100 hover:bg-slate-200'} rounded px-3 py-2 transition-all duration-200 cursor-pointer`}>
                      <div className="w-3 h-3 rounded-full bg-slate-700 border border-slate-500" />
                      <span className={`${darkMode ? 'text-slate-300' : 'text-slate-600'} text-sm`}>Black</span>
                      <span className={`${darkMode ? 'text-white' : 'text-slate-800'} font-medium ml-auto`}>{metrics?.lineCounts?.B || 0}</span>
                    </div>
                  </div>
                </div>

              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}
