package com.example.mobile_project

import android.util.Log
import androidx.recyclerview.widget.RecyclerView
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView

import com.example.mobile_project.databinding.FragmentDisplayPlantsItemBinding
import com.squareup.picasso.Picasso
import java.io.File
import java.util.*
import android.content.Intent

import android.graphics.Color
import androidx.activity.result.ActivityResultLauncher
import androidx.core.text.HtmlCompat
import java.text.SimpleDateFormat


/**
 * [RecyclerView.Adapter] that can display a plant
 */
class MyPlantsRecyclerViewAdapter(
    private var values : List<Plant>,
    private var nextWatering : List<String>,
    private val prefix : String,
    private var launcher : ActivityResultLauncher<Intent>,
    private var callback : AdapterCallback
) : RecyclerView.Adapter<MyPlantsRecyclerViewAdapter.ViewHolder>() {


    /**
     * Returns a formatted non null String from a date with time
     */
    private fun getStringFromDateTime(date : String): String {
        val calendar: Calendar = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
        calendar.time = simpleDateFormat.parse(date)

        val year = String.format("%02d", calendar.get(Calendar.YEAR))
        val month = String.format("%02d", calendar.get(Calendar.MONTH) + 1)
        val day = String.format("%02d", calendar.get(Calendar.DAY_OF_MONTH))
        val hour = String.format("%02d", calendar.get(Calendar.HOUR_OF_DAY))
        val minute = String.format("%02d", calendar.get(Calendar.MINUTE))

        return "$day/$month/$year $hour:$minute"
    }

    /**
     * Returns a formatted non null String from a date (without time)
     */
    private fun getStringFromDate(date : String?): String {
        if (date == null)
            return ""

        val calendar: Calendar = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.FRENCH)
        calendar.time = simpleDateFormat.parse(date)

        val year = String.format("%02d", calendar.get(Calendar.YEAR))
        val month = String.format("%02d", calendar.get(Calendar.MONTH) + 1)
        val day = String.format("%02d", calendar.get(Calendar.DAY_OF_MONTH))

        return "$day/$month/$year"
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        return ViewHolder(
            FragmentDisplayPlantsItemBinding.inflate(
                LayoutInflater.from(parent.context),
                parent,
                false
            )
        )
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = values[position]

        // Set background colors
        holder.itemView.setBackgroundColor(
            if (position % 2 == 0)
                Color.parseColor("#84A98C")
            else
                Color.parseColor("#52796F")
        )

        // Load Image from gallery
        if (item.image != null && !item.image.equals("@drawable/no_image")) {
            val f = File(item.image)
            if (f.exists()) {
                Log.d("imgPath", "img exists")
                Picasso.with(holder.imgView.context)
                    .load(f)
                    .into(holder.imgView)
            }
        } else { // Reset default image (to avoid Picasso cache showing wrong images)
            val imgUri = "https://www.adenine-rh.com/wp-content" +
                    "/themes/consultix/images/no-image-found-360x250.png"
            Picasso.with(holder.imgView.context)
                .load(imgUri)
                .into(holder.imgView)
        }

        // Change color of  Nom and Latin (when searching)
        // prefix is the prefix but it respects the Upper and Lower cases of the name
        var prefixName = ""
        var prefixLatin = ""
        // Suffix is the name without its prefix
        var suffixName = ""
        var suffixLatin = ""

        if (item.nom != null) {
            prefixName = item.nom.substring(0, prefix.length)
            suffixName = item.nom.substring(prefix.length)
        }
        if (item.latin != null) {
            prefixLatin = item.latin.substring(0, prefix.length)
            suffixLatin = item.latin.substring(prefix.length)
        }

        // Prefix in the search bar is highlighted in another color
        val newName : String = "<font color=#cc0029>" +
                prefixName +
                "</font>" +
                suffixName
        val newLatin : String = "<font color=#cc0029>" +
                prefixLatin +
                "</font>" +
                suffixLatin

        // Data to display
        holder.idView.text = item.idPlant.toString()
        holder.name.text =
            when {
                prefixName.equals(prefix, true) ->
                    HtmlCompat.fromHtml(newName, HtmlCompat.FROM_HTML_MODE_LEGACY)
                item.nom != null -> item.nom.toString()
                else -> ""
            }
        holder.latin.text =
            when {
                prefixLatin.equals(prefix, true) ->
                    HtmlCompat.fromHtml(newLatin, HtmlCompat.FROM_HTML_MODE_LEGACY)
                item.latin != null -> item.latin.toString()
                else -> ""
            }

        holder.last.text = getStringFromDateTime(item.last_watering)
        holder.next.text = getStringFromDate(nextWatering[position])
        holder.freq.text = item.frequence.toString()
        holder.season.text =
            if (item.frequence2 == null)
                ""
            else
                item.frequence2.toString()
        holder.seasonFrom.text = getStringFromDate(item.season_from)
        holder.seasonTo.text = getStringFromDate(item.season_to)
        holder.nutri.text =
            if (item.frequence3 == null)
                ""
            else
                item.frequence3.toString()
        holder.nutriFrom.text = getStringFromDate(item.nutrient_from)
        holder.nutriTo.text = getStringFromDate(item.nutrient_to)

        // Click on Edit icon : Launch new Activity to edit current plant
        holder.editImgView.setOnClickListener {
            val intent = Intent(holder.itemView.context, UpdatePlantActivity::class.java)
            // Add Plant data
            intent.putExtra("idPlant", item.idPlant)
            intent.putExtra("nom", item.nom)
            intent.putExtra("latin", item.latin)
            intent.putExtra("freq", item.frequence)
            intent.putExtra("freq2", item.frequence2)
            intent.putExtra("freqNutri", item.frequence3)
            intent.putExtra("bLastWatering", item.last_watering)
            intent.putExtra("bFreqFrom", item.season_from)
            intent.putExtra("bFreqTo", item.season_to)
            intent.putExtra("bFreqNutriFrom", item.nutrient_from)
            intent.putExtra("bFreqNutriTo", item.nutrient_to)
            intent.putExtra("imgPath", item.image)

            launcher.launch(intent)
        }

        // Click on Watering Icon : waters the plant
        holder.waterImgView.setOnClickListener {
            val calendar: Calendar = Calendar.getInstance()
            val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
            val currentDate = simpleDateFormat.format(calendar.time)

            val thisPlant =
                Plant(item.idPlant, item.nom, item.latin, item.frequence,
                    item.frequence2, item.season_from, item.season_to,
                    item.frequence3, item.nutrient_from, item.nutrient_to,
                    currentDate, item.image)

            callback.updateCallback(thisPlant, position) // Parent method to update plant
        }
    }

    override fun getItemCount(): Int = values.size

    inner class ViewHolder(binding: FragmentDisplayPlantsItemBinding) :
        RecyclerView.ViewHolder(binding.root) {
        val idView: TextView = binding.idPlant
        val imgView: ImageView = binding.itemImage
        val name: TextView = binding.name
        val latin : TextView = binding.latin
        val last : TextView = binding.last
        val next : TextView = binding.next
        val freq : TextView = binding.frequence
        val season : TextView = binding.freqSeason
        val seasonFrom : TextView = binding.seasonFrom
        val seasonTo : TextView = binding.seasonTo
        val nutri : TextView = binding.freqNutri
        val nutriFrom : TextView = binding.nutriFrom
        val nutriTo : TextView = binding.nutriTo

        val editImgView : ImageView = binding.editIcon
        val waterImgView : ImageView = binding.waterIcon

        override fun toString(): String {
            return super.toString() + " '" + idView.text + "'"
        }
    }
}