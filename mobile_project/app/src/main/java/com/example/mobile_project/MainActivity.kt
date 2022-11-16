package com.example.mobile_project

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import android.view.View
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider

import androidx.viewpager2.widget.ViewPager2
import com.example.mobile_project.databinding.ActivityMainBinding
import com.google.android.material.tabs.TabLayoutMediator
import java.util.*
import kotlin.math.abs


/**
 * Main Activity of the project
 * This Activity showcases the 2 fragments :
 * DisplayPlantsFragments and AddPlantFragment
 */
class MainActivity : AppCompatActivity() {

    private val binding : ActivityMainBinding by lazy{ ActivityMainBinding.inflate(layoutInflater) }
    private lateinit var model : ViewModel

    private var launcher : ActivityResultLauncher<Intent> =
        registerForActivityResult(
            ActivityResultContracts.StartActivityForResult()) {
                model.loadAll()
            }

    @RequiresApi(Build.VERSION_CODES.M)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        // Setting Action Bar
        setSupportActionBar(binding.myToolbar)

        //val action: String? = intent?.action
        val data: Uri? = intent?.data
        Log.d("qrurl", data.toString())
        //if (data.toString() != "null")
        //    Toast.makeText(this, data.toString(), Toast.LENGTH_SHORT).show()



        model = ViewModelProvider(this).get(ViewModel::class.java)
        val names = listOf(getString(R.string.display), getString(R.string.add))

        val afficherFragment = DisplayPlantsFragment.newInstance()

        val ajouterFragment = AddPlantFragment.newInstance(data.toString())

        val pagerAdapter = ScreenSlidePagerAdapter(
            this,
            mutableListOf(afficherFragment, ajouterFragment)
        )
        binding.pager.adapter = pagerAdapter

        /* donner les noms au tabs et les attacher aux fragments correspondants*/

        TabLayoutMediator(binding.tabLayout, binding.pager) { tab, position ->
            tab.text = names[position]
        }.attach()

        binding.fab.setOnClickListener {
            binding.pager.currentItem = 1
        }

        //pager.adapter
        binding.pager.registerOnPageChangeCallback(object : ViewPager2.OnPageChangeCallback() {
            override fun onPageSelected(position: Int) {
                if (position == 0) {
                    binding.fab.show()
                }
                else if (position == 1) {
                    binding.fab.hide()
                }
                super.onPageSelected(position)
            }
        })

        myAlarmManager() // Daily alarm Manager
    }

    /**
     * Menu Toolbar
     */
    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        val inflater: MenuInflater = menuInflater
        inflater.inflate(R.menu.activity_main_menu, menu)

        return super.onCreateOptionsMenu(menu)
    }

    /**
     *  Menu Toolbar items actions
     */
    override fun onOptionsItemSelected(item: MenuItem): Boolean = when (item.itemId) {
        R.id.waterPlants -> {
            // OnClick Watering Icon in Menu Toolbar will launch Watering Activity
            val wateringIntent = Intent(this, WateringActivity::class.java)
            launcher.launch(wateringIntent)
            true
        }
        R.id.quitter -> {
            finish()
            true
        }
        else -> {
            super.onOptionsItemSelected(item)
        }
    }

    /* sans onBackPressed() le bouton back termine (finish()) l'activité */
    override fun onBackPressed() {
        if (binding.pager.currentItem == 0) {
            // If the user is currently looking at the first step, allow the system to handle the
            // Back button. This calls finish() on this activity and pops the back stack.
            super.onBackPressed()
            //binding.pager.currentItem = binding.pager.adapter!!.getItemCount() - 1
        } else {
            // Otherwise, select the previous step.
            binding.pager.currentItem = binding.pager.currentItem - 1
        }
    }

    /**
     * Schedules daily alarms at 8 AM
     * which will start a Notification Service
     */
    @RequiresApi(Build.VERSION_CODES.M)
    private fun myAlarmManager() {
        // Intent to put in pendingIntent
        val serviceIntent = Intent(this, MyNotificationService::class.java)

        // Creating pendingIntent with the serviceIntent inside
        val pendingIntent =
            PendingIntent.getService(
                this, 1,
                serviceIntent, PendingIntent.FLAG_IMMUTABLE)

        // Get reference to AlarmManager
        val alarmManager = getSystemService(Context.ALARM_SERVICE) as AlarmManager

        val alarmHourTime = 8 // 24 HOUR FORMAT

        // Create Calendar of today 8 AM
        val calendar : Calendar = Calendar.getInstance().apply {
            timeInMillis = System.currentTimeMillis()
            set(Calendar.HOUR_OF_DAY, alarmHourTime)
        }

        /**
         * Prevents the alarm from firing immediately
         * if the current time is past the alarm time
         *
         * Note : We keep it commented to get a notification
         * shortly after launching the app
         */
        //if (Calendar.getInstance().after(calendar))
          //  calendar.add(Calendar.DATE, 1)

        // Repeat Alarm everyday at calendar time
        alarmManager.setInexactRepeating(
            AlarmManager.RTC_WAKEUP,
            calendar.timeInMillis,
            AlarmManager.INTERVAL_DAY,
            pendingIntent
        )
    }
}


class DepthPageTransformer : ViewPager2.PageTransformer {
    companion object{
        private const val MIN_SCALE = 0.75f
    }
    override fun transformPage(view: View, position: Float) {
        view.apply {
            val pageWidth = width
            when {
                position < -1 -> { // [-Infinity,-1)
                    // This page is way off-screen to the left.
                    alpha = 0f
                }
                position <= 0 -> { // [-1,0]
                    // Use the default slide transition when moving to the left page
                    alpha = 1f
                    translationX = 0f
                    scaleX = 1f
                    scaleY = 1f
                }
                position <= 1 -> { // (0,1]
                    // Fade the page out.
                    alpha = 1 - position

                    // Counteract the default slide transition
                    translationX = pageWidth * -position

                    // Scale the page down (between MIN_SCALE and 1)
                    val scaleFactor = (MIN_SCALE + (1 - MIN_SCALE) * (1 - abs(position)))
                    scaleX = scaleFactor
                    scaleY = scaleFactor
                }
                else -> { // (1,+Infinity]
                    // This page is way off-screen to the right.
                    alpha = 0f
                }
            }
        }
    }
}