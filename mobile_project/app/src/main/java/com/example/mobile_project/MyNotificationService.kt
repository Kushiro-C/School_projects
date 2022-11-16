package com.example.mobile_project

import android.annotation.SuppressLint
import android.app.*
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.IBinder
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.app.NotificationCompat
import java.text.SimpleDateFormat
import java.util.*


/**
 * Service creating and putting a notification in foreground.
 * This notification launches the WateringActivity
 */
class MyNotificationService : Service() {
    private val CHANNEL_ID = "watering message"
    private val dao = PlantDatabase.getDatabase(this).myDao()


    override fun onBind(intent: Intent): IBinder? {
        return null
    }

    // Notification Channel API 26+
    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val name = "MyNotifChannel"
            val descriptionText = "description"
            val importance = NotificationManager.IMPORTANCE_DEFAULT
            val channel = NotificationChannel(CHANNEL_ID, name, importance).apply {
                description = descriptionText
            }

            // Register the channel with the system
            val notificationManager: NotificationManager =
                getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    /**
     * Checks in our database if there are plants to water
     */
    private fun hasNextWatering() : Boolean {
        var list : List<Plant> = emptyList()
        val t = Thread {
            dao.loadAll().toList().also { list = it }
        }
        t.start()
        t.join()

        // Init Calendars and Frequencies
        val todayCalendar = Calendar.getInstance()
        val nextWatering = Calendar.getInstance()
        val lastWatering = Calendar.getInstance()
        val seasonFrom = Calendar.getInstance()
        val seasonTo = Calendar.getInstance()
        val nutrientFrom = Calendar.getInstance()
        val nutrientTo = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.FRENCH)

        list.forEach {

            // Set Frequencies
            val freq = it.frequence
            val seasonFreq = it.frequence2 ?: 0
            val nutrientFreq = it.frequence3 ?: 0

            // Set Calendars
            lastWatering.time = simpleDateFormat.parse(it.last_watering)
            if (seasonFreq > 0) { // Not null if seasonFreq is set
                seasonFrom.time = simpleDateFormat.parse(it.season_from)
                seasonTo.time = simpleDateFormat.parse(it.season_to)
            }
            if (nutrientFreq > 0) { // Not null if nutrientFreq is set
                nutrientFrom.time = simpleDateFormat.parse(it.nutrient_from)
                nutrientTo.time = simpleDateFormat.parse(it.nutrient_to)
            }

            // Set nextWatering date
            nextWatering.time = lastWatering.time
            if (todayCalendar in nutrientFrom..nutrientTo
                    && nutrientFreq > 0) // Priority on nutrient
                nextWatering.add(Calendar.DATE, nutrientFreq)

            else if (todayCalendar in seasonFrom..seasonTo
                    && seasonFreq > 0) // Priority season
                nextWatering.add(Calendar.DATE, seasonFreq)

            else // Normal watering frequency
                nextWatering.add(Calendar.DATE, freq)

            // Check if we are past the nextWatering date
            if (todayCalendar.after(nextWatering))
                return true
        }

        Log.d("hasNextWatering", "No plants to water : return false")

        // No plants to water
        return false
    }

    @SuppressLint("UnspecifiedImmutableFlag")
    @RequiresApi(Build.VERSION_CODES.M)
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent != null) {
            val value = intent.getStringExtra("cancel")
            if (value == "STOP") {
                Log.d("cancel", "Notification closed")
                stopSelf()
                stopForeground(true)
                return START_NOT_STICKY
            }
        }

        createNotificationChannel()

        val notifyIntent =
            Intent(this, WateringActivity::class.java).apply {
                setFlags(Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK)
            }

        val notifyPendingIntent = PendingIntent.getActivity(
            this, 0, notifyIntent, PendingIntent.FLAG_UPDATE_CURRENT
        )

        // Cancel intent
        val cancelIntent = Intent(this, MyNotificationService::class.java)
            .putExtra("cancel", "STOP")
        val pendingIntentCancel = PendingIntent.getService(this, 2,
            cancelIntent, PendingIntent.FLAG_IMMUTABLE)

        // Create the notification
        val notification = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(getString(R.string.app_name))
            .setContentText(getString(R.string.notif_text))
            .setContentIntent(notifyPendingIntent)
            .setAutoCancel(true)
            .setSmallIcon(R.drawable.plant_icon)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT) // Android <= 7.1
            .addAction(
                R.drawable.plant_icon, getString(R.string.close),
                pendingIntentCancel
            )
            .build()

        // Display notification if there are plants to water
        if (hasNextWatering()) {
            Log.d("watering", "We have plants to water !")
            startForeground(startId, notification)
        }

        return START_STICKY
    }
}