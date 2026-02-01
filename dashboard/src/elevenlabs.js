/**
 * ElevenLabs Text-to-Speech Service
 *
 * Provides integration with ElevenLabs API for converting text to natural speech.
 * Used by the Arctic Analytics dashboard to power "Penny the Penguin" - the AI
 * voice guide that explains dashboard metrics to users.
 *
 * API Documentation: https://elevenlabs.io/docs/api-reference/text-to-speech
 *
 * @module elevenlabs
 * @author Arctic Analytics Team
 * @version 1.0.0
 */


/* =============================================================================
   CONFIGURATION
   ============================================================================= */

/**
 * ElevenLabs API base URL for text-to-speech conversion
 * @constant {string}
 */
const ELEVENLABS_API_URL = 'https://api.elevenlabs.io/v1/text-to-speech'

/**
 * Default voice ID for TTS output
 * "George" - A natural, friendly narrator voice suitable for explanations
 *
 * Browse available voices: https://elevenlabs.io/docs/voices
 * @constant {string}
 */
const DEFAULT_VOICE_ID = 'JBFqnCBsd6RMkjVDRZzb'


/* =============================================================================
   API FUNCTIONS
   ============================================================================= */

/**
 * Convert text to speech using ElevenLabs API
 *
 * Makes a POST request to the ElevenLabs text-to-speech endpoint with the
 * provided text and voice settings. Returns the audio as an MP3 blob.
 *
 * @async
 * @param {string} text - The text content to convert to speech
 * @param {string} apiKey - ElevenLabs API key (from environment: VITE_ELEVENLABS_API_KEY)
 * @param {Object} [options={}] - Optional voice configuration settings
 * @param {string} [options.voiceId] - Voice ID to use (default: George narrator)
 * @param {string} [options.modelId='eleven_multilingual_v2'] - TTS model to use
 * @param {number} [options.stability=0.5] - Voice stability (0-1, higher = more consistent)
 * @param {number} [options.similarityBoost=0.75] - Voice clarity boost (0-1)
 * @param {number} [options.style=0] - Style exaggeration (0-1)
 * @param {number} [options.speed=1.0] - Playback speed multiplier
 *
 * @returns {Promise<Blob>} Audio blob in MP3 format (44.1kHz, 128kbps)
 *
 * @throws {Error} If API request fails or returns non-OK status
 *
 * @example
 * const audioBlob = await textToSpeech("Hello world!", apiKey);
 * playAudioBlob(audioBlob);
 */
export async function textToSpeech(text, apiKey, options = {}) {
  // Destructure options with defaults
  const {
    voiceId = DEFAULT_VOICE_ID,
    modelId = 'eleven_multilingual_v2',
    stability = 0.5,
    similarityBoost = 0.75,
    style = 0,
    speed = 1.0
  } = options

  // Make API request
  const response = await fetch(`${ELEVENLABS_API_URL}/${voiceId}?output_format=mp3_44100_128`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'xi-api-key': apiKey  // ElevenLabs uses xi-api-key header for auth
    },
    body: JSON.stringify({
      text,
      model_id: modelId,
      voice_settings: {
        stability,
        similarity_boost: similarityBoost,
        style,
        speed,
        use_speaker_boost: true  // Enhances voice clarity
      }
    })
  })

  // Handle API errors
  if (!response.ok) {
    const error = await response.json().catch(() => ({}))
    throw new Error(error.detail?.message || `ElevenLabs API error: ${response.status}`)
  }

  // Return audio as blob
  return await response.blob()
}


/* =============================================================================
   AUDIO PLAYBACK
   ============================================================================= */

/**
 * Play an audio blob through the browser's audio system
 *
 * Creates a temporary object URL for the blob, plays the audio,
 * and automatically cleans up the URL when playback completes.
 *
 * @param {Blob} audioBlob - Audio data blob (typically MP3 from textToSpeech)
 *
 * @returns {HTMLAudioElement} Audio element for playback control (pause, stop, etc.)
 *
 * @example
 * const audio = playAudioBlob(audioBlob);
 * // Later: audio.pause() to stop playback
 */
export function playAudioBlob(audioBlob) {
  // Create temporary URL for the blob
  const audioUrl = URL.createObjectURL(audioBlob)
  const audio = new Audio(audioUrl)

  // Clean up object URL when audio finishes to prevent memory leaks
  audio.onended = () => {
    URL.revokeObjectURL(audioUrl)
  }

  // Start playback
  audio.play()

  // Return element for external control
  return audio
}


/* =============================================================================
   CONTENT GENERATION
   ============================================================================= */

/**
 * Generate a spoken explanation of the dashboard based on current metrics
 *
 * Creates a natural language script for Penny the Penguin to read,
 * dynamically incorporating the current sensor data and session information.
 *
 * @param {Object|null} metrics - Calculated dashboard metrics from App.jsx
 * @param {string} metrics.duration - Session duration in seconds
 * @param {string} metrics.avgDistance - Average obstacle distance in cm
 * @param {string} metrics.avgSpeed - Average motor speed in RPM
 * @param {Object} metrics.colorCounts - Color detection counts {R, G, B, U}
 * @param {Object} metrics.lineCounts - Line detection counts {W, B}
 * @param {Object|null} sessionInfo - Session metadata from MongoDB
 * @param {string} sessionInfo.robot_id - Robot identifier
 *
 * @returns {string} Natural language explanation script for TTS
 *
 * @example
 * const script = generateDashboardExplanation(metrics, sessionInfo);
 * const audio = await textToSpeech(script, apiKey);
 */
export function generateDashboardExplanation(metrics, sessionInfo) {
  // Handle loading state
  if (!metrics) {
    return "Welcome to Arctic Analytics! I'm your penguin guide. The dashboard is currently loading data. Please wait a moment for the sensor readings to appear."
  }

  // Determine most detected color
  const colorMost = Object.entries(metrics.colorCounts || {})
    .sort((a, b) => b[1] - a[1])[0]

  const colorNames = { R: 'red', G: 'green', B: 'blue', U: 'unknown' }
  const dominantColor = colorNames[colorMost?.[0]] || 'unknown'

  // Determine dominant line reading
  const lineMost = (metrics.lineCounts?.W || 0) > (metrics.lineCounts?.B || 0) ? 'white' : 'black'

  // Build explanation script
  return `Welcome to Arctic Analytics! I'm Penny the Penguin, your dashboard guide.

Let me explain what you're seeing. This dashboard displays real-time sensor data from robot ${sessionInfo?.robot_id || 'arctic-1'}.

In the top section, you'll find three key metrics: The session duration is ${metrics.duration} seconds, with an average obstacle distance of ${metrics.avgDistance} centimeters detected by the ultrasonic sensor, and the motors are running at an average speed of ${metrics.avgSpeed} RPM.

The main chart shows sensor data over time, tracking both distance measurements and motor speeds. The green line represents distance to obstacles, while the red line shows left motor speed.

On the right side, you can see two pie charts. The Color Detection chart shows what colors the robot's color sensor has detected. Currently, ${dominantColor} is the most detected color. The Line Following chart displays the IR sensor readings, showing the robot has mostly been over ${lineMost} surfaces.

The Motor Comparison bar chart at the bottom left compares left and right motor speeds, which helps identify any steering imbalances.

That's your Arctic Analytics overview! Click me anytime for more information.`
}
